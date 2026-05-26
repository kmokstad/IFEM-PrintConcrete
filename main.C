// $Id$
//==============================================================================
//!
//! \file main.C
//!
//! \date May 26 2026
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Main program for the isogeometric simulator of 3D concrete printing.
//!
//==============================================================================

#include "SIMFiniteDefEl.h"
#include "SIM2D.h"
#include "SIM3D.h"
#include "NonlinearDriver.h"
#include "HDF5Writer.h"
#include "HDF5Restart.h"
#include "Profiler.h"
#include "NLargs.h"
#include "IFEM.h"
#include <filesystem>
#include <cstdlib>
#include <cstring>


/*!
  \brief Reads the input file and invokes the main simulation driver.
*/

int runSimulator (SIMoutput& model, char* infile, int form,
                  double stopTime, double zero_tol, int outPrec)
{
  NonlinearDriver simulator(model, form == SIM::LINEAR);

  utl::profiler->start("Model input");

  // Read in solver and model definitions
  if (!simulator.read(infile))
    return 1;

  // Let the stop time specified on command-line override input file setting
  if (stopTime > 0.0)
    simulator.setStopTime(stopTime);

  model.opt.print(IFEM::cout,true) << std::endl;
  simulator.printProblem();

  utl::profiler->stop("Model input");

  // Preprocess the model and establish data structures for the algebraic system
  if (!model.preprocess())
    return 2;

  if (model.opt.format >= 0)
  {
    // Save FE model to VTF file for visualization
    if (model.getNoSpaceDim() < 3)
      model.opt.nViz[2] = 1;
    if (!simulator.saveModel(infile))
      return 4;
    else if (stopTime < 0.0 && !model.writeGlvStep(1))
      return 4;
  }

  if (stopTime < 0.0)
    return 0; // model check

  if (model.opt.discretization < ASM::Spline && !model.opt.hdf5.empty())
  {
    IFEM::cout <<"\n ** HDF5 output is available for spline discretization only"
               <<". Deactivating...\n"<< std::endl;
    model.opt.hdf5.clear();
  }

  // If more than one projection method is specified, use only the first one
  const char* projectType = nullptr;
  if (!model.opt.project.empty())
    projectType = model.opt.project.begin()->second.c_str();

  // Define the initial configuration
  simulator.initPrm();
  simulator.initSol(2);
  simulator.initProj(projectType ? 1 : 0);

  // Initialize the linear equation solver
  if (!simulator.initEqSystem(true,model.getNoFields()))
    return 3;

  // Load solution state from serialized data in case of restart
  if (!simulator.checkForRestart())
    return 5;

  // Open HDF5 result database
  DataExporter* writer = nullptr;
  if (model.opt.dumpHDF5(infile))
  {
    const std::string& fileName = model.opt.hdf5;
    IFEM::cout <<"\nWriting HDF5 file "<< fileName <<".hdf5"<< std::endl;

    // Include secondary results only if no projection has been requested.
    // The secondary results will be projected anyway, but without the
    // nodal averaging across patch boundaries in case of multiple patches.
    int results = DataExporter::PRIMARY;
    if (!projectType && !model.opt.pSolOnly)
      results |= DataExporter::SECONDARY;
    if (model.opt.saveNorms)
      results |= DataExporter::NORMS;
    if (model.hasElementActivator())
      results |= DataExporter::ELEMENT_MASK;

    writer = new DataExporter(true,model.opt.saveInc);
    writer->registerWriter(new HDF5Writer(fileName,model.getProcessAdm()));
    writer->registerField("u","solution",DataExporter::SIM,results);
    writer->setFieldValue("u",&model,&simulator.getSolution(),
                          nullptr,simulator.getNorms());
    if (projectType)
    {
      writer->registerField("sigma","projected",DataExporter::SIM,
                            DataExporter::SECONDARY,projectType);
      writer->setFieldValue("sigma",&model,simulator.getProjection());
    }
  }

  HDF5Restart* restart = nullptr;
  if (model.opt.restartInc > 0)
  {
    std::string hdf5file(infile);
    if (!model.opt.hdf5.empty())
      hdf5file = model.opt.hdf5 + "_restart";
    else
      hdf5file.replace(hdf5file.find_last_of('.'),std::string::npos,"_restart");
    const size_t idot = hdf5file.size();
    for (int i = 1; std::filesystem::exists(hdf5file + ".hdf5"); i++)
      hdf5file = hdf5file.substr(0,idot) + std::to_string(i);
    IFEM::cout <<"\nWriting HDF5 file "<< hdf5file <<".hdf5"<< std::endl;
    restart = new HDF5Restart(hdf5file,model.getProcessAdm(),
                              model.opt.restartInc);
  }

  if (projectType)
    IFEM::cout <<"\n"<< projectType <<" will be used to compute"
               <<"\nsmoothed secondary solution fields."<< std::endl;

  // Now invoke the main solution driver
  int status = simulator.solveProblem(writer,restart,zero_tol,outPrec);
  delete writer;
  delete restart;
  return status;
}


/*!
  \brief Main program for the isogeometric finite deformation solver.

  The input to the program is specified through the following
  command-line arguments. The arguments may be given in arbitrary order.

  \arg \a input-file : Input file with model definition
  \arg -dense :   Use the dense LAPACK matrix equation solver
  \arg -spr :     Use the SPR direct equation solver
  \arg -superlu : Use the sparse SuperLU equation solver
  \arg -samg :    Use the sparse algebraic multi-grid equation solver
  \arg -petsc :   Use equation solver from PETSc library
  \arg -nGauss \a n : Number of Gauss points over a knot-span in each direction
  \arg -vtf \a format : VTF-file format (-1=NONE, 0=ASCII, 1=BINARY)
  \arg -nviz \a nviz : Number of visualization points over each knot-span
  \arg -nu \a nu : Number of visualization points per knot-span in u-direction
  \arg -nv \a nv : Number of visualization points per knot-span in v-direction
  \arg -nw \a nw : Number of visualization points per knot-span in w-direction
  \arg -hdf5 : Write primary and projected secondary solution to HDF5 file
  \arg -saveInc \a dtSave : Time increment between each result save to VTF/HDF5
  \arg -outPrec \a nDigit : Number of digits in solution component printout
  \arg -ztol \a eps : Zero tolerance for printing of solution norms
  \arg -check : Data check only, read model and output to VTF (no solution)
  \arg -stopTime \a t : Run simulation only up to specified stop time
  \arg -2D : Use two-parametric simulation driver (plane stress)
  \arg -2Dpstrain : Use two-parametric simulation driver (plane strain)
  \arg -UL : Use updated Lagrangian formulation with nonlinear material
  \arg -MX<pord> : Mixed formulation with internal discontinuous pressure
  \arg -mixed : Mixed formulation with continuous pressure and volumetric change
  \arg -Mixed : Same as -mixed, but use C^(p-1) continuous displacement basis
  \arg -Fbar<nvp> : Use the F-bar formulation
  \arg -linear : Do a linear analysis only (no iterations)
*/

int main (int argc, char** argv)
{
  Profiler prof(argv[0]);

  int outPrec = 3;
  double zero_tol = 1.0e-8;
  double stopTime = 0.0;
  char* infile = nullptr;
  NLargs args;

  IFEM::Init(argc,argv,"Concrete 3D printing simulator");

  for (int i = 1; i < argc; i++)
    if (argv[i] == infile || args.parseArg(argv[i]))
      ; // ignore the input file on the second pass
    else if (SIMoptions::ignoreOldOptions(argc,argv,i))
      ; // ignore the obsolete option
    else if (!strcmp(argv[i],"-outPrec") && i < argc-1)
      outPrec = atoi(argv[++i]);
    else if (!strcmp(argv[i],"-ztol") && i < argc-1)
      zero_tol = atof(argv[++i]);
    else if (!strcmp(argv[i],"-stopTime") && i < argc-1)
      stopTime = atof(argv[++i]);
    else if (!strcmp(argv[i],"-check"))
      stopTime = -1.0;
    else if (!infile && strcasestr(argv[i],".xinp"))
    {
      infile = argv[i];
      if (args.readXML(infile,false))
        i = 0; // start over and let command-line options override input file
      else
        return 1; // pre-parse failure
    }
    else
      std::cerr <<"  ** Unknown option ignored: "<< argv[i] << std::endl;

  if (!infile)
  {
    std::cout <<"usage: "<< argv[0]
	      <<" <inputfile> [-dense|-spr|-superlu[<nt>]|-samg|-petsc]\n"
	      <<"       [-2D[pstrain] [-nGauss <n>]"
	      <<" [-UL|-MX[<p>]|-[M|m]ixed|-Fbar<nvp>] [-linear]\n"
	      <<"       [-hdf5 [<filename>] [-vtf <format> [-nviz <nviz>]"
	      <<" [-nu <nu>] [-nv <nv>] [-nw <nw>]]\n"
	      <<"       [-saveInc <dtSave>] [-check] [-stopTime <t>]"
              <<" [-outPrec <nd>] [-ztol <eps>]\n";
    return 0;
  }

  IFEM::cout <<"\nInput file: "<< infile;
  IFEM::getOptions().print(IFEM::cout);
  if (outPrec != 3)
    IFEM::cout <<"\nNorm- and component output precision: "<< outPrec;
  if (zero_tol != 1.0e-8)
    IFEM::cout <<"\nNorm output zero tolerance: "<< zero_tol;
  IFEM::cout << std::endl;

  std::vector<int> options;
  if (args.pOrd >= 0)
    options = { args.form, args.pOrd };
  else if (args.form >= 0)
    options = { args.form };

  if (args.dim == 2)
  {
    SIMFiniteDefEl<SIM3D> model(false,options);
    return runSimulator(model,infile,args.form,stopTime,zero_tol,outPrec);
  }
  else
  {
    SIMFiniteDefEl<SIM2D> model(false,options);
    return runSimulator(model,infile,args.form,stopTime,zero_tol,outPrec);
  }
}
