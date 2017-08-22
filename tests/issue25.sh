#!/usr/bin/env bash
# tests groff functionality
# RFE#25

# OFILE=`mktemp`
SS=issue25.oleo
REP=issue25.rep
neoleo $SS -H <<< tbl | groff -T ascii -t | head -10 > out/$REP
#mv $OFILE out/ref.oleo
diff out/$REP verified/$REP
exit $?
