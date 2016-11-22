#!/bin/bash

source /opt/qt56/bin/qt56-env.sh
mkdir build
cd build
cmake .. -DWITH_COVERAGE=ON
make -j4 
ctest -V
