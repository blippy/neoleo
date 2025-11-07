#!/bin/sh
# 2025-11-06 Added
# Test of get-cell-fmt

NEO=`realpath ../src/neoleo`
REP=tcl08.rep


$NEO -m tcl  <<- "EOF" >out/$REP 
set-cell 2 3 "124"
set v [get-cell-fmt 2 3]
puts "formatted cell is '$v'"
puts "Expecting         '     124'"
EOF

diff out/$REP verified/$REP
