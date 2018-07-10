#!/bin/bash

# exit when any command fails
set -e

cmake_command="cmake .. -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE"

if [ "$WITH_COVERAGE" == "ON" ]; then
    cmake_command="$cmake_command -DWITH_COVERAGE=ON"
fi

echo "CMake: $cmake_command"
echo "ci_env: $ci_env"
sudo modprobe vcan
docker run \
     $ci_env -e WITH_COVERAGE -u cds -e CC -e CXX -e DISPLAY=:10 --privileged --cap-add=ALL \
     -v `pwd`:/home/sources rkollataj/candevstudio /bin/bash \
     -c "sudo ip link add name can0 type vcan && sudo ip link set can0 up && ip link && sudo service xvfb start && sudo chown cds:cds /home/sources && cd /home/sources && rm -rf build && mkdir -p build && cd build && source /opt/qt58/bin/qt58-env.sh &&
        $cmake_command &&
        make -j5 &&
        make test &&
        if [ '$WITH_COVERAGE' == 'ON' ]; then bash <(curl -s https://codecov.io/bash) -x gcov-6 || echo 'Codecov did not collect coverage reports'; fi &&
        if [ '$PACKAGE' == 'ON' ]; then 
            cpack -G DEB &&
            cmake . -DSTANDALONE=ON &&
            cpack -G TXZ &&
            mkdir ../master && mv *.deb *.tar.xz ../master &&
            cmake . -DDEV_BUILD=OFF &&
            cpack -G TXZ &&
            cmake . -DSTANDALONE=OFF &&
            cpack -G DEB &&
            mkdir ../rc && mv *.deb *.tar.xz ../rc || echo 'Failed to create package'; fi"

if [ "$PACKAGE" == "ON" ]; then
    cd master
    for f in *; do
        curl -T $f -urkollataj:$BINTRAY_API_KEY https://api.bintray.com/content/rkollataj/CANdevStudio/master/${TRAVIS_COMMIT:0:7}/$f\;override=1
    done
    curl -urkollataj:$BINTRAY_API_KEY -X POST https://api.bintray.com/content/rkollataj/CANdevStudio/master/${TRAVIS_COMMIT:0:7}/publish

    cd ../rc
    for f in *; do
        curl -T $f -urkollataj:$BINTRAY_API_KEY https://api.bintray.com/content/rkollataj/CANdevStudio/cds/rc/$f\;override=1
    done
    curl -urkollataj:$BINTRAY_API_KEY -X POST https://api.bintray.com/content/rkollataj/CANdevStudio/cds/rc/publish
fi
