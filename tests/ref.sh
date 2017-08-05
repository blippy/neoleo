#!/usr/bin/env bash
# a simple test of the referencing of neoleo
# Just verifies that the input matches the output
# and that no regression has been introduced
OFILE=`mktemp`
neoleo ../examples/ref.oleo -H <<< "save-spreadsheet $OFILE"
mv $OFILE out/test-suite.oleo
diff out/test-suite.oleo verified/test-suite.oleo
exit $?
