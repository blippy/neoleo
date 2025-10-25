#!/bin/sh

NEO=`realpath ../src/neoleo`
$NEO -H  << EOF
! rm -f out/insert-01.oleo
i
12
13.1
sum(r1:[-1]c1)
.
g 1 1
ri
g 1 1
i
42
.
w out/insert-01.oleo
EOF

diff out/insert-01.oleo verified/insert-01.scr.oleo
