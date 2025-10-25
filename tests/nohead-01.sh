#!/bin/sh

NEO=`realpath ../src/neoleo`
$NEO -H << EOF
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

diff out/nohead-01.oleo verified/nohead-01.scr.oleo