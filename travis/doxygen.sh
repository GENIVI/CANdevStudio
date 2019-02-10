#!/bin/bash

set -e

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
  cd "$TRAVIS_BUILD_DIR" || exit
  chmod +x .travis_doxygen.sh
  docker run \
    $ci_env -e DOXYFILE -e GH_REPO_NAME -e GH_REPO_REF -e GH_REPO_TOKEN -e TRAVIS_BUILD_NUMBER -u cds \
    -v `pwd`:/home/sources rkollataj/candevstudio /bin/bash \
    -c "sudo chown cds:cds /home/sources && cd /home/sources && ./.travis_doxygen.sh"
fi
