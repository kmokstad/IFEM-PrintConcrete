## Introduction

This is a simulator for 3D printing of concrete structures,
developed by SINTEF Digital in the SCENE-B project.
The simulator is derived from the FiniteDeformation parts of the
[IFEM-Elasticity](https://github.com/OPM/IFEM-Elasticity) module,
and adds features for time-dependent element activation and a
linear elastic material model with time-dependent Drucker-Prager yield criterion.

### Getting all dependencies

1. Install IFEM from https://github.com/OPM/IFEM

### Getting the code

This is done by first navigating to a folder `<App root>` in which you want
the application and typing

    git clone https://github.com/OPM/IFEM-Elasticity
    git clone https://github.com/kmokstad/IFEM-PrintConcrete

The build system uses sibling directory logic to locate the IFEM-Elasticity module.

### Compiling the code

To compile, first navigate to the root catalogue `<App root>`.

    cd IFEM-PrintConcrete
    mkdir Release
    cd Release
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make

This will compile the libraries and the PrintConcrete application.
The executable can be found in the 'bin' sub-folder.
Change all instances of `Release` with `Debug` to build with debug-symbols.

### Testing the code

To compile and run all regression- and unit-tests, navigate to your build
folder (i.e., `<App root>/IFEM-PrintConcrete/Release`) and type

    make check
