#!/usr/bin/env bash
OFILE=`mktemp`
#echo $OFILE
neoleo ../examples/ref.oleo -H <<< "save-spreadsheet $OFILE"
mv $OFILE out/test-suite.oleo
diff out/test-suite.oleo verified/test-suite.oleo
exit $?
