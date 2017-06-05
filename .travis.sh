#!/bin/bash

cmake_command="cmake /root/sources -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE"

if [ "$WITH_COVERAGE" == "ON" ]; then
    cmake_command="$cmake_command -DWITH_COVERAGE=ON"
fi

echo "CMake: $cmake_command"
docker run -e WITH_COVERAGE=$WITH_COVERAGE -e CXX=$CXX -v `pwd`:/root/sources bartekt/can_dev_base_image /bin/bash -c \
     "rm -rf /root/build && mkdir -p /root/build && cd /root/build && source /opt/qt58/bin/qt58-env.sh &&
      $cmake_command && 
      make -j5 &&
      make test && 
      if [ '$WITH_COVERAGE' == 'ON' ]; then bash <(curl -s https://codecov.io/bash) -x gcov-6 || echo 'Codecov did not collect coverage reports'; fi"
