#!/bin/sh

which dpkg > /dev/null && dpkg -l | grep libxcb-xinerama0 > /dev/null || echo "\nWARNING: libxcbx-xinerama0 may be required by libqxcb.so plugin. You may want to execute 'sudo apt-get install libxcb-xinerama0'\n"

appname=`basename $0 | sed s,\.sh$,,`

dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi
LD_LIBRARY_PATH=$dirname
export LD_LIBRARY_PATH
$dirname/$appname "$@"
