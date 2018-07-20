#!/bin/bash

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
    echo refs/heads/$TRAVIS_BRANCH~ refs/heads/$TRAVIS_BRANCH refs/heads/$TRAVIS_BRANCH | ./.trigger_appveyor.rb || true
    export ci_env=`bash <(curl -s https://codecov.io/env)`
else
    brew install qt5 ninja
fi

