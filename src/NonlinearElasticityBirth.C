// $Id$
//==============================================================================
//!
//! \file NonlinearElasticityBirth.C
//!
//! \date Aug 18 2026
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Integrand implementations for nonlinear elasticity problems.
//!
//==============================================================================

#include "NonlinearElasticityBirth.h"


void NonlinearElasticityBirth::initIntegration (size_t nGp, size_t nBp)
{
  Fbirth.resize(nGp,Tensor(nDF));

  this->NonlinearElasticityUL::initIntegration(nGp,nBp);
}


void NonlinearElasticityBirth::initIntegration (const TimeDomain& prm,
                                                const Vector&, bool)
{
  iAmIntegrating = true;

  this->NonlinearElasticityUL::initIntegration(prm,{},false);
}


void NonlinearElasticityBirth::initResultPoints (double lambda, char prinDir)
{
  iAmIntegrating = false;

  this->NonlinearElasticityUL::initResultPoints(lambda,prinDir);
}


bool NonlinearElasticityBirth::kinematics (const Vector& eV, size_t iP,
                                           const Vector& N, const Matrix& dNdX,
                                           double r, Tensor& F, Matrix*,
                                           SymmTensor* E) const
{
  if (!iAmIntegrating)
    return this->NonlinearElasticityUL::kinematics(eV,0,N,dNdX,r,F,nullptr,E);

  if (E)
    E->zero();

  if (iP >= Fbirth.size())
  {
    std::cerr <<" *** NonlinearElasticityBirth::kinematics:"
              <<" Integration point "<< iP+1 <<" is out of range [1,"
              << Fbirth.size() <<"]."<< std::endl;
    return false;
  }

  Tensor& F0 = Fbirth[iP];
  if (F0.isZero(1.0e-16))
  {
    // Find the deformation gradient at the element birth configuration
    if (!this->NonlinearElasticityUL::kinematics(eV,0,N,dNdX,r,F0))
      return false;
    else if (F0.inverse() <= 0.0)
      return false;

    F = 1.0; // Stress-free element birth configuration
  }
  else
  {
    // Find deformation gradient w.r.t. the element birth configuration
    if (!this->NonlinearElasticityUL::kinematics(eV,0,N,dNdX,r,F))
      return false;

    F *= F0;
  }

  if (E)
  {
    // Form the Green-Lagrange strain tensor
    Tensor F1(F); F1 -= 1.0;
    for (unsigned short int i = 1; i <= E->dim(); i++)
      for (unsigned short int j = 1; j <= i; j++)
      {
        double Eij = F1(i,j) + F1(j,i);
        for (unsigned short int k = 1; k <= nsd; k++)
          Eij += F1(k,i)*F1(k,j);
        (*E)(i,j) = i == j ? 0.5*Eij : Eij;
      }
  }

#if INT_DEBUG > 0
  std::cout <<"NonlinearElasticityBirth::F =\n"<< F;
#endif
  return true;
}


bool NonlinearElasticityBirth::checkItgBuffer (size_t nGp) const
{
  if (nGp == Fbirth.size())
    return this->NonlinearElasticityUL::checkItgBuffer(nGp);

  std::cerr <<" *** NonlinearElasticityBirth::checkItgBuffer:"
            <<" Can't change number number of integration points, "
            << nGp <<" != "<< Fbirth.size() << std::endl;
  return false;
}
