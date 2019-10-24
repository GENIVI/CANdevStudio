#!/bin/bash

set -e

# Execute for GENIVI repository only. Exclude PR.
if [ "$TRAVIS_OS_NAME" == "linux" -a "$TRAVIS_REPO_SLUG" == "GENIVI/CANdevStudio" -a "$TRAVIS_PULL_REQUEST" == "false" -a "$DOXYGEN" == "ON"]; then
  cd "$TRAVIS_BUILD_DIR" || exit
  chmod +x .travis_doxygen.sh
  docker run \
    $ci_env -e DOXYFILE -e GH_REPO_NAME -e GH_REPO_REF -e GH_REPO_TOKEN -e TRAVIS_BUILD_NUMBER -u cds \
    -v `pwd`:/home/sources rkollataj/candevstudio /bin/bash \
    -c "sudo chown cds:cds /home/sources && cd /home/sources && ./.travis_doxygen.sh"
fi
