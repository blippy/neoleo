#!/bin/sh
# 2025-10-30	Added

NEO=`realpath ../src/neoleo`
REP=tcl2.rep

$NEO -m tcl  <<- "EOF" >out/$REP 
set meaning [ploppy-life]
puts "Meaning of life is $meaning . Expecting 42"
EOF

diff out/$REP verified/$REP
