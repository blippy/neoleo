#!/usr/bin/env bash
# a simple test of the referencing of neoleo
# Just verifies that the input matches the output
# and that no regression has been introduced
TMPFILE=`mktemp`
SS=$ASRCDIR/ref01.oleo
echo "SS=$SS"
neoleo $SS -H <<< "save-spreadsheet $TMPFILE"
set_productions $SS
mv $TMPFILE $OUTFILE
diff $OUTFILE $VERFILE
exit $?
