#!/bin/sh

$BUILDDIR/src/neoleo -H << EOF
# test of issue 18
# cell referencing parsing bug

! rm -f out/issue18.oleo
i
10
r1c1+1
.
w out/issue18.oleo
EOF

diff out/issue18.oleo $SRCDIR/verified/issue18.scr.oleo