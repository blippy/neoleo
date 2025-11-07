#!/bin/sh
# 2025-10-30	Added
# Test get-cell

NEO=`realpath ../src/neoleo`
REP=tcl04.rep

$NEO -m tcl  sht1.oleo <<- "EOF" >out/$REP 
puts "Running tcl4.tcl"

set v [get-cell 1 2]
puts "cell(1,2) is $v. Expecting 12"

set v [get-cell 2 2]
puts "cell(2,2) is $v. Expecting hello world"

EOF

diff out/$REP verified/$REP
