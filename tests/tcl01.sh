#!/bin/sh

NEO=`realpath ../src/neoleo`
REP=tcl01.rep

$NEO -m tcl  <<- "EOF" >out/$REP 
puts "tcl1 test"
hi-ploppy
puts "end of test"
EOF

diff out/$REP verified/$REP
