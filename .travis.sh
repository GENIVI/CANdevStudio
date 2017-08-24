#!/bin/bash

cmake_command="cmake .. -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE"

if [ "$WITH_COVERAGE" == "ON" ]; then
    cmake_command="$cmake_command -DWITH_COVERAGE=ON"
fi

echo "CMake: $cmake_command"
echo "ci_env: $ci_env"
docker run \
     $ci_env -e WITH_COVERAGE -e CC -e CXX -e DISPLAY=:10\
     -v `pwd`:/root/sources przemyslawdraszkiewicz/can_dev_base_image /bin/bash \
     -c "service xvfb start && cd /root/sources && rm -rf build && mkdir -p build && cd build && source /opt/qt58/bin/qt58-env.sh &&
        $cmake_command &&
        make -j5 &&
        make test &&
        if [ '$WITH_COVERAGE' == 'ON' ]; then bash <(curl -s https://codecov.io/bash) -x gcov-6 || echo 'Codecov did not collect coverage reports'; fi"
