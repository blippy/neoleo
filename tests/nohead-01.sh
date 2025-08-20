#!/bin/sh

$BUILDDIR/src/neoleo -H << EOF
! rm -f out/nohead-01.oleo
i
12
13.1
"hello world"
;
rc[-1] + 2
rc[-1] + 3.3
.
w out/nohead-01.oleo
EOF

diff out/nohead-01.oleo $SRCDIR/verified/nohead-01.scr.oleo