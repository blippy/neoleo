#!/bin/sh
# 2025-10-30	Added

NEO=`realpath ../src/neoleo`
REP=tcl3.rep

$NEO -m tcl  sht1.oleo <<- "EOF" >out/$REP 
set mr [max-row]
puts "Expecting max-row 3, got $mr"

set mc [max-col]
puts "Expecting max-col 2, got $mc"
EOF

diff out/$REP verified/$REP