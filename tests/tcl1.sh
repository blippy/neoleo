#!/bin/sh

NEO=`realpath ../src/neoleo`
REP=tcl1.tcl.rep
$NEO -0 -t tcl1.tcl  >out/$REP
diff out/$REP verified/$REP
