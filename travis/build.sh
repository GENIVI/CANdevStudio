#!/bin/bash

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
  export CC=gcc-6 CXX=g++-6 CMAKE_BUILD_TYPE=Debug WITH_COVERAGE=ON PACKAGE=OFF
  ./travis/docker_compile.sh
  export CC=gcc-53 CXX=g++-53 CMAKE_BUILD_TYPE=Release WITH_COVERAGE=OFF PACKAGE=ON
  ./travis/docker_compile.sh
  export CC=gcc-53 CXX=g++-53 CMAKE_BUILD_TYPE=Debug WITH_COVERAGE=OFF PACKAGE=OFF
  ./travis/docker_compile.sh
  export CC=clang-3.5 CXX=clang++-3.5 CMAKE_BUILD_TYPE=Release WITH_COVERAGE=OFF PACKAGE=OFF
  ./travis/docker_compile.sh
  export CC=clang-3.5 CXX=clang++-3.5 CMAKE_BUILD_TYPE=Debug WITH_COVERAGE=OFF PACKAGE=OFF
  ./travis/docker_compile.sh
else
    echo "Building for OSX"
    cmake -H. -Bbuild-osx -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt/5.11.1/lib/cmake -GNinja -DWITH_TESTS=OFF -DCMAKE_INSTALL_PREFIX=install
    cmake --build build-osx --config Release
    cmake --build build-osx --config Release --target install
    cmake --build build-osx --config Release --target install
fi
