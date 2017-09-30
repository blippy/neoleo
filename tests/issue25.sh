#!/usr/bin/env bash
# tests groff functionality
# RFE#25

# OFILE=`mktemp`
TMPFILE=`mktemp`
SS=$ASRCDIR/issue25.oleo
echo "SS=$SS"
#REP=issue25.rep
neoleo $SS -H <<< tbl | groff -T ascii -t | head -10 > $TMPFILE
set_productions issue25.rep
#mv $OFILE out/ref.oleo
mv $TMPFILE $OUTFILE
diff $OUTFILE $VERFILE
exit $?
