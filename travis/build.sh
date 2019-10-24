#!/bin/bash

set -e

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    ./travis/docker_compile.sh
else

    chmod -R 755 "$(brew --prefix qt5)/lib/"
    echo "Building for OSX"
    cmake -H. -Bbuild-osx -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$(brew --prefix qt5)/lib/cmake" -GNinja -DWITH_TESTS=ON -DCMAKE_INSTALL_PREFIX=install

    cmake --build build-osx --config Release
    # First install command always fails for some reason...
    cmake --build build-osx --config Release --target install || true
    cmake --build build-osx --config Release --target install

    # cd causes shell script to interrupt. Ignore errors onwards
    set +e
    cd build-osx
    ctest

    if [ "$TRAVIS_PULL_REQUEST" == "false" ] && [ "$TRAVIS_BRANCH" == "master" ]; then
        # DEV_BUILD=ON (master)
        cpack -G DragNDrop
        mkdir master
        mv *.dmg master

        # Deploy (master)
        cd master
        for f in *.dmg; do
            curl -T $f -urkollataj:$BINTRAY_API_KEY https://api.bintray.com/content/rkollataj/CANdevStudio/master/${TRAVIS_COMMIT:0:7}/$f\;override=1
        done
        curl -urkollataj:$BINTRAY_API_KEY -X POST https://api.bintray.com/content/rkollataj/CANdevStudio/master/${TRAVIS_COMMIT:0:7}/publish
        cd ..

        # DEV_BUILD=OFF (rc)
        cmake . -DDEV_BUILD=OFF
        cpack -G DragNDrop
        mkdir rc
        mv *.dmg rc

        # Deploy (rc)
        cd rc
        for f in *.dmg; do
            curl -T $f -urkollataj:$BINTRAY_API_KEY https://api.bintray.com/content/rkollataj/CANdevStudio/cds/rc/$f\;override=1
        done
        curl -urkollataj:$BINTRAY_API_KEY -X POST https://api.bintray.com/content/rkollataj/CANdevStudio/cds/rc/publish
        cd ..
    fi
fi
