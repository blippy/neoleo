#!/bin/sh

NEO=`realpath ../src/neoleo`
REP=tcl2.tcl.rep
$NEO -0 -t tcl2.tcl  >out/$REP
diff out/$REP verified/$REP
