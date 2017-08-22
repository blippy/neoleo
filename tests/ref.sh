#!/usr/bin/env bash
# a simple test of the referencing of neoleo
# Just verifies that the input matches the output
# and that no regression has been introduced
OFILE=`mktemp`
SS=ref.oleo
neoleo $SS -H <<< "save-spreadsheet $OFILE"
mv $OFILE out/$SS
diff out/$SS verified/$SS
exit $?
