// $Id$
//==============================================================================
//!
//! \file NLargs.C
//!
//! \date Nov 20 2024
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Pre-parsing of input files for Finite Deformation applications.
//!
//==============================================================================

#include "NLargs.h"
#include "NLoptions.h"
#include "SIMenums.h"
#include "ASMmxBase.h"
#include "ElasticityUtils.h"
#include "Utilities.h"
#include "IFEM.h"
#include "tinyxml2.h"


bool NLargs::parseArg (const char* argv)
{
  if (!strncmp(argv,"-2Dpstra",8))
  {
    dim = 2;
    Elastic::planeStrain = true;
  }
  else if (!strcmp(argv,"-linear"))
    form = SIM::LINEAR;
  else if (!strcmp(argv,"-UL"))
  {
    if (form < SIM::UPDATED_LAGRANGE)
      form = SIM::UPDATED_LAGRANGE;
  }
  else if (!strncmp(argv,"-MX",3))
  {
    form = SIM::MIXED_QnPn1;
    if (strlen(argv) > 3 && isdigit(argv[3]))
      pOrd = atoi(argv+3);
  }
  else if (!strcmp(argv,"-Mixed"))
  {
    form = SIM::MIXED_QnQn1;
    ASMmxBase::Type = ASMmxBase::FULL_CONT_RAISE_BASIS1;
  }
  else if (!strcmp(argv,"-mixed"))
  {
    form = SIM::MIXED_QnQn1;
    ASMmxBase::Type = ASMmxBase::REDUCED_CONT_RAISE_BASIS1;
  }
  else if (!strncmp(argv,"-Fbar",5))
  {
    form = SIM::FBAR;
    if (strlen(argv) > 5 && isdigit(argv[5]))
      pOrd = atoi(argv+5);
  }
  else
    return this->SIMargsBase::parseArg(argv);

  return true;
}


bool NLargs::parse (const tinyxml2::XMLElement* elem)
{
  if (!strcasecmp(elem->Value(),"finitedeformation"))
  {
    const tinyxml2::XMLElement* child = elem->FirstChildElement("formulation");
    if (child) this->parseFormulation(child);
  }

  return this->SIMargsBase::parse(elem);
}


void NLargs::parseFormulation (const tinyxml2::XMLElement* elem)
{
  const tinyxml2::XMLElement* child = elem->FirstChildElement();
  for (; child; child = child->NextSiblingElement())
    if (!strcasecmp(child->Value(),"planestrain"))
      Elastic::planeStrain = dim == 2;
    else if (!strcasecmp(child->Value(),"totallagrange"))
      form = SIM::TOTAL_LAGRANGE;
    else if (!strcasecmp(child->Value(),"updatedlagrange") &&
             form < SIM::UPDATED_LAGRANGE)
      form = SIM::UPDATED_LAGRANGE;
    else if (!strcasecmp(child->Value(),"linear"))
      form = SIM::LINEAR;
    else if (!strcasecmp(child->Value(),"mixed"))
    {
      if (child->FirstChild())
        pOrd = atoi(child->FirstChild()->Value());
      std::string type;
      utl::getAttribute(child,"type",type);
      if (type == "Qp/Pp-1")
        form = SIM::MIXED_QnPn1;
      else if (type == "Qp/Qp-1")
      {
        form = SIM::MIXED_QnQn1;
        if (pOrd == 1)
          ASMmxBase::Type = ASMmxBase::FULL_CONT_RAISE_BASIS1;
        else
          ASMmxBase::Type = ASMmxBase::REDUCED_CONT_RAISE_BASIS1;
      }
      else if (type == "Fbar")
        form = SIM::FBAR;
    }
}
