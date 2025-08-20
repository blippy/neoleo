#!/bin/sh

$BUILDDIR/src/neoleo -H << EOF
# 2025-04-20 A simple script to see if headless writing works
! rm -f out/write.oleo
I
1
2
3
.
w out/write.oleo
EOF

diff out/write.oleo $SRCDIR/verified/write.scr.oleo