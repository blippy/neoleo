#!/bin/sh

$BUILDDIR/src/neoleo -H << EOF
# BUG#16 blank line test
! rm -f out/issue16.oleo
i
1

2
.

w out/issue16.oleo
EOF

diff out/issue16.oleo $SRCDIR/verified/issue16.scr.oleo