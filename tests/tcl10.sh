#!/bin/sh

OLEO=tcl10.oleo
rm -f out/$OLEO

NEO=`realpath ../src/neoleo`
$NEO  -m tcl <<- "EOF"
# 2025-04-20 A simple script to see if Tcl writing works

insert-by-row
1
2
3
.
save-oleo-as out/tcl10.oleo
EOF

diff out/$OLEO verified/$OLEO