#!/usr/bin/env bash
# test parser created in 2019

# OFILE=`mktemp`
TMPFILE=`mktemp`
#SS=$ASRCDIR/issue25.oleo
#echo "SS=$SS"
#REP=issue25.rep
NEO=$ABUILDDIR/../src/neoleo
#echo "NEO=$NEO"
#$NEO $SS -H <<< tbl | groff -T ascii -t | head -10 > $TMPFILE
$NEO -T parser2019 > $TMPFILE
set_productions parser.rep
#mv $OFILE out/ref.oleo
mv $TMPFILE $OUTFILE
diff $OUTFILE $VERFILE
exit $?
