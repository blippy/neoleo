#!/bin/sh
# 2025-10-31 Added
# Test of set-cell

NEO=`realpath ../src/neoleo`
REP=tcl6.rep

$NEO -m tcl  <<- "EOF" >out/$REP 
set-cell 2 3 "24"
set v [get-cell 2 3]
puts "Got $v. Expected 24"

set-cell 2 4 "\"hello world\""
set v [get-cell 2 4]
puts "Got $v. Expected hello world"

set-cell 2 5 "r2c3+1"
set v [get-cell 2 5]
puts "Got $v. Expected 25"
EOF

diff out/$REP verified/$REP