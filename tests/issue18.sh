#!/bin/sh

NEO=`realpath ../src/neoleo`
echo "Looking for neoleo in $NEO"
echo "Current working dir $PWD"
$NEO -m h << EOF
# test of issue 18
# cell referencing parsing bug

! rm -f out/issue18.oleo
i
10
r1c1+1
.
w out/issue18.oleo
EOF

diff out/issue18.oleo verified/issue18.scr.oleo
