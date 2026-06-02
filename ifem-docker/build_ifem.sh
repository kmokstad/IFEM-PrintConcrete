#!/bin/bash

set -e

git clone -b $1 https://github.com/OPM/IFEM
cd IFEM/Apps
git clone -b $2 https://github.com/OPM/IFEM-Elasticity
git clone -b $3 https://github.com/kmokstad/IFEM-PrintConcrete

mkdir IFEM-PrintConcrete/Release
cd IFEM-PrintConcrete/Release
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release -DIFEM_AS_SUBMODULE=1 \
-DIFEM_USE_OPENMP=ON -DIFEM_USE_HDF5=ON -DIFEM_USE_SPR=ON \
-DIFEM_USE_LRSPLINES=OFF -DIFEM_USE_PETSC=OFF -DIFEM_USE_ISTL=OFF \
-DIFEM_USE_VTFWRITER=OFF
ninja
ninja install
cd ../../..
rm IFEM -Rf
