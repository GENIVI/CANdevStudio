#!/bin/bash

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
  cd "$TRAVIS_BUILD_DIR" || exit
  chmod +x .travis_doxygen.sh
  docker run
  $ci_env -e DOXYFILE -e GH_REPO_NAME -e GH_REPO_REF -e GH_REPO_TOKEN -e TRAVIS_BUILD_NUMBER
  -v `pwd`:/root/sources rkollataj/candevstudio /bin/bash
  -c "cd /root/sources && ./.travis_doxygen.sh"
fi
