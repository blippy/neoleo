#!/usr/bin/env bash
OFILE=`mktemp`
neoleo  -H $OFILE < $1

#echo "param $1"
#DFILE=nohead-01.oleo
DFILE=$1.oleo
mv $OFILE out/$DFILE
diff out/$DFILE verified/$DFILE
exit $?
