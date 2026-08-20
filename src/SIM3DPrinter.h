// $Id$
//==============================================================================
//!
//! \file SIM3DPrinter.h
//!
//! \date May 26 2026
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Solution driver for 3D printing of concrete structures.
//!
//==============================================================================

#ifndef _SIM_3D_PRINTER_H
#define _SIM_3D_PRINTER_H

#include "SIMFiniteDefEl.h"

class IntFunc;


/*!
  \brief Driver class for isogeometric 3D printing simulation.
*/

template<class Dim>
class SIM3DPrinter : public SIMFiniteDefEl<Dim>
{
public:
  //! \brief The default constructor forwards to the parent class constructor.
  explicit SIM3DPrinter(bool fb = false)
    : SIMFiniteDefEl<Dim>(false,{SIM::UPDATED_LAGRANGE}), useFbirth(fb) {}

  //! \brief Prints out problem-specific data to the log stream.
  bool printProblem() const override;

protected:
  //! \brief Parses an element activation function.
  //! \param[in] func Function definition
  //! \param[in] type Function type
  IntFunc* parseElemActivator(const std::string& func,
                              const std::string& type) const override;
  //! \brief Parses material data from an XML element.
  //! \param[in] elem The XML element to parse
  Material* parseMaterial(const tinyxml2::XMLElement* elem) override;

  //! \brief Returns the actual integrand.
  ElasticBase* getIntegrand() override;

private:
  bool useFbirth; //!< If \e true, use the NonlinearElasticityBirth integrand
};

#endif
