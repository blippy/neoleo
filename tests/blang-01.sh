#!/bin/sh

NEO=`realpath ../src/neoleo`
REP=blang-01.bas.rep
$NEO -H -b blang-01.bas  >out/$REP
diff out/$REP verified/$REP
