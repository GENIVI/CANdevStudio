#!/bin/sh

appname=`basename $0 | sed s,\.sh$,,`

dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi

# Support standard Linux installation path
[ -f "$dirname/../lib/CANdevStudio/$appname" ] && dirname="$dirname/../lib/CANdevStudio"

LD_LIBRARY_PATH=$dirname
export LD_LIBRARY_PATH
$dirname/$appname "$@"
