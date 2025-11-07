#!/bin/sh

NEO=`realpath ../src/neoleo`
OLEO=issue18.oleo

rm -f out/$OLEO

$NEO -m tcl <<- "EOF"
# test of issue 18
# cell referencing parsing bug


insert-by-col
10
r1c1+1
.
save-oleo-as out/issue18.oleo
EOF

diff out/$OLEO verified/$OLEO
