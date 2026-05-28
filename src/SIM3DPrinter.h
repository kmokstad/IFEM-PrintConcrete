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
  SIM3DPrinter() : SIMFiniteDefEl<Dim>(false,{SIM::UPDATED_LAGRANGE}) {}

  //! \brief Prints out problem-specific data to the log stream.
  virtual bool printProblem() const;

protected:
  //! \brief Parses an element activation function.
  //! \param[in] func Function definition
  //! \param[in] type Function type
  virtual IntFunc* parseElemActivator(const std::string& func,
                                      const std::string& type) const;
  //! \brief Parses material data from an XML element.
  //! \param[in] elem The XML element to parse
  virtual Material* parseMaterial(const tinyxml2::XMLElement* elem);
};

#endif
