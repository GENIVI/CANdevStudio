#!/bin/bash

set -e

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
    export ci_env=`bash <(curl -s https://codecov.io/env)`
else
    brew install qt5 ninja
fi

