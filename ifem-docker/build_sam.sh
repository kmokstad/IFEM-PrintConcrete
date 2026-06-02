#!/bin/bash

set -e

git clone --recurse-submodules https://github.com/openfedem/sam-lib

mkdir sam-lib/Release
cd sam-lib/Release
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBUILD_SPR_INTEGER8=OFF
ninja
ninja install
cd ../..
rm sam-lib -Rf
