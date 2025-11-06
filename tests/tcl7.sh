#!/bin/sh
# 2025-10-31 Added
# Test of save-oleo-as

NEO=`realpath ../src/neoleo`
OLEO=tcl7.oleo
REP=tcl7.rep

rm out/$REP out/$OLEO

$NEO -m tcl  <<- "EOF" >out/$REP 
set-cell 2 3 "124"
save-oleo-as out/tcl7.oleo
EOF

diff out/$OLEO verified/$OLEO