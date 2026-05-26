// $Id$
//==============================================================================
//!
//! \file NLargs.h
//!
//! \date Nov 20 2024
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Pre-parsing of input files for Finite Deformation applications.
//!
//==============================================================================

#ifndef _NL_ARGS_H
#define _NL_ARGS_H

#include "SIMargsBase.h"


/*!
  \brief Class for input file pre-parsing and command-line argument values.
*/

class NLargs : public SIMargsBase
{
public:
  //! \brief Default constructor.
  explicit NLargs() : SIMargsBase("finitedeformation"), form(-1), pOrd(-1) {}
  //! \brief Parses a command-line argument.
  virtual bool parseArg(const char* argv);

protected:
  //! \brief Parses a data section from an XML element.
  virtual bool parse(const tinyxml2::XMLElement* elem);

private:
  //! \brief Parses the formulation tag from an XML element.
  void parseFormulation(const tinyxml2::XMLElement* elem);

public:
  int form; //!< Nonlinear formulation option
  int pOrd; //!< Order of internal pressure field
};

#endif
