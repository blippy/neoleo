#!/bin/sh

# 2025-10-13 Check blang setting and getting cells
# Never run this from within the tests directory
# because it basically wonks up all the settings

# SRCDIR should point to the tests directory 

echo "Incoming SRCDIR=$SRCDIR"
#if [ "$SRCDIR" = "" ]
#then
#	SRCDIR=$PWD
#fi

#if [ "$SRCDIR" = "." ]
#then
#	SRCDIR=`realpath ..`
#fi
SRCDIR=`realpath $SRCDIR`
echo "Resolved SRCDIR=$SRCDIR"


#if [ "$BUILDDIR" = "" ]
#then
#	BUILDDIR=$SRCDIR
#fi
echo "Incoming BUILDDIR=$BUILDDIR"
BUILDDIR=`realpath $BUILDDIR`
echo "Resolved BUILDDIR=$BUILDDIR"
NEO="$BUILDDIR/src/neoleo"
echo "NEO=$NEO"
BASE=blang-01.bas
REP=$BASE.rep
CMD="$NEO -H -b $SRCDIR/$BASE"
echo "CMD:$CMD"
TESTDIR=$BUILDDIR/tests
echo "TESTDIR=$TESTDIR"
$CMD >$TESTDIR/out/$REP


NEO=`realpath ../src/neoleo`
REP=blang-01.bas.rep
$NEO -H -b blang-01.bas  >out/$REP
diff out/$REP verified/$REP
