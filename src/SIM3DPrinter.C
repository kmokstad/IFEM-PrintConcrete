// $Id$
//==============================================================================
//!
//! \file SIM3DPrinter.C
//!
//! \date May 26 2026
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Solution driver for 3D printing of concrete structures.
//!
//==============================================================================

#include "SIM3DPrinter.h"
#include "NonlinearElasticityBirth.h"
#include "DruckerPrager.h"

#include "IFEM.h"
#include "SIM2D.h"
#include "SIM3D.h"
#include "Functions.h"
#include "tinyxml2.h"


namespace
{
  // This is a linear function for x > 1 otherwise equal to f0.
  class Linear : public IntFunc
  {
  public:
    Linear(Real s, Real offset) : IntFunc(s,offset) {}

  protected:
    Real evaluate(const int& x) const override
    {
      return x > 1 || f0 > Real(0) ? this->IntFunc::evaluate(x) : f0;
    }
  };

  // This class is used as an element activation function, where a bunch of
  // elements (nv) is activated in the first parameter direction, assuming the
  // number of elements in the first direction is nu. If nw equals 1, all nv
  // elements in the v-w parameter plane are activated instantly in one step.
  // Otherwise, the nv elements are divided into nw groups, each consisting
  // of nv/nw elements, which then are activated one by one.
  class Parametric : public IntFunc
  {
    int n0; // Always ignore the first n0 elements
    int nu; // Number of elements in first parameter direction
    int nv; // Number of elements to activate in v-w plane before advancing in u
    int nw; // Number of element divisions per layer in w-direction

  public:
    explicit Parametric(int u, int v, int w, Real scale)
      : IntFunc(scale), n0(0), nu(u), nv(v), nw(w) {}
    explicit Parametric(int ignore_init, int u, Real offset, Real scale)
      : IntFunc(scale,offset), n0(ignore_init), nu(u), nv(1), nw(1) {}

  protected:
    Real evaluate(const int& x) const override
    {
      if (x <= n0) return 1.0e99; // always ignore this element

      int ix = (x-1)%nu;
      int nx = (x-1)/nu;
      if (ix < 1 && nx < nv) return f0;

      Real delta = nw > 1 && nw < nv ? Real(1+nx/(nv/nw))/Real(nw) : Real(0);
      return f0 + sf*(Real(ix + nu*(nx/nv)) + delta);
    }
  };
}


template<class Dim>
IntFunc* SIM3DPrinter<Dim>::parseElemActivator (const std::string& func,
                                                const std::string& type) const
{
  if (func.empty())
  {
    IFEM::cout <<"  ** SIM3DPrinter::parseElmActivator: No function"
               <<" parameters specified - returning identity"<< std::endl;
    return new Linear(1.0,0.0);
  }

  IntFunc* f = nullptr;
  char* prms = strdup(func.c_str());
  char* cstr = nullptr;

  if (type == "parametric")
  {
    int ne1 = atoi(strtok(prms," "));
    int ne2 = (cstr = strtok(nullptr," ")) ? atoi(cstr) : 1;
    int ne3 = (cstr = strtok(nullptr," ")) ? atoi(cstr) : 1;
    Real sf = (cstr = strtok(nullptr," ")) ? atof(cstr) : Real(1);
    f = new Parametric(ne1,ne2,ne3,sf);
  }
  else if (type == "offset")
  {
    int ne1 = atoi(strtok(prms," "));
    Real ts = (cstr = strtok(nullptr," ")) ? atof(cstr) : Real(0);
    Real sf = (cstr = strtok(nullptr," ")) ? atof(cstr) : Real(1);
    int ne0 = (cstr = strtok(nullptr," ")) ? atoi(cstr) : 0;
    f = new Parametric(ne0,ne1,ts,sf);
  }
  else
  {
    // This will take simple expressions on the form <a>*t+/-<b>
    // where <a> and <b> are numerical values, and not much else
    Real scaling = atof(strtok(prms," *t"));
    Real offset  = (cstr = strtok(nullptr," *t")) ? atof(cstr) : Real(0);
    f = new Linear(scaling,offset);
  }

  free(prms);
  return f;
}


template<class Dim> Material*
SIM3DPrinter<Dim>::parseMaterial (const tinyxml2::XMLElement* elem)
{
  if (!strcasecmp(elem->Value(),"druckerprager"))
  {
    int code = this->parseMaterialSet(elem,SIMElasticity<Dim>::mVec.size());
    IFEM::cout <<"\tMaterial code "<< code;

    Material* mat = new DruckerPrager(Dim::dimension,true);
    mat->parse(elem);
    IFEM::cout << std::endl;
    return mat;
  }
  else
    return this->SIMFiniteDefEl<Dim>::parseMaterial(elem);
}


template<class Dim>
ElasticBase* SIM3DPrinter<Dim>::getIntegrand ()
{
  if (Dim::myProblem || !useFbirth)
    return this->SIMFiniteDefEl<Dim>::getIntegrand();

  Dim::myProblem = new NonlinearElasticityBirth(Dim::dimension);

  return dynamic_cast<ElasticBase*>(Dim::myProblem);
}


template<class Dim>
bool SIM3DPrinter<Dim>::printProblem () const
{
  if (!this->SIMFiniteDefEl<Dim>::printProblem())
    return false;

  IFEM::cout <<"3D printing simulator"<< std::endl;
  return true;
}


template class SIM3DPrinter<SIM2D>;
template class SIM3DPrinter<SIM3D>;
