#!/bin/sh

NEO=`realpath ../src/neoleo`
REP=tcl1.rep
$NEO -m 0 -t tcl1.tcl  >out/$REP
diff out/$REP verified/$REP
