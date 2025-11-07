#!/bin/sh
# 2025-10-30 Added. Test load-oleo

NEO=`realpath ../src/neoleo`
REP=tcl05.rep

$NEO -m tcl  <<- "EOF" >out/$REP 
load-oleo "sht1.oleo"

puts "max rows [max-row]"
set v [get-cell 1 2]
puts "Value is $v. Expected 12"
EOF

diff out/$REP verified/$REP
