#!/bin/sh
# 2025-111-6 Added
# Test of save-oleo-as

NEO=`realpath ../src/neoleo`
OLEO=tcl07.oleo
REP=tcl07.rep

rm out/$REP out/$OLEO

$NEO -m tcl  <<- "EOF" >out/$REP 
set-cell 2 3 "124"
save-oleo-as out/tcl07.oleo
EOF

diff out/$OLEO verified/$OLEO
