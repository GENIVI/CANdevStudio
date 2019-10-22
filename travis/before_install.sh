#!/bin/bash

set -e

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
    export ci_env=`bash <(curl -s https://codecov.io/env)`
else
    HOMEBREW_NO_AUTO_UPDATE=1 brew install qt5 ninja
fi

