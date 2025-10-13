#!/bin/sh

# 2025-10-13 Check blang setting and getting cells

# TODO incorporate in 
if [ "$BUILDDIR" = "" ]
then
	if [ -d "../src" ] 
	then
		BUILDDIR=".."
	fi
fi

if [ "$SRCDIR" = "" ]
then
	SRCDIR=$BUILDDIR/tests
fi

echo "SRCDIR:$SRCDIR"
BASE=blang-01.bas
REP=$BASE.rep
CMD="$BUILDDIR/src/neoleo -H -b $SRCDIR/$BASE"
echo "CMD:$CMD"
$CMD >$SRCDIR/out/$REP

diff out/$REP $SRCDIR/verified/$REP
