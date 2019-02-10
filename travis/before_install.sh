#!/bin/bash

set -e

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
    echo refs/heads/$TRAVIS_BRANCH~ refs/heads/$TRAVIS_BRANCH refs/heads/$TRAVIS_BRANCH | ./.trigger_appveyor.rb || true
    export ci_env=`bash <(curl -s https://codecov.io/env)`
else
    brew install qt@5.11 ninja
fi

