#!/usr/bin/env bash

# TODO use  the functinality provided in neotests-env.sh
#source neotests-env.sh

TMPFILE=`mktemp`

THE_BUILDDIR=`pwd`
THE_SRCDIR=`pwd`

echo "SRCDIR=$SRCDIR"

while getopts "b:s:" opt
do
	case $opt in
		b) THE_BUILDDIR="$OPTARG" ;;
		s) THE_SRCDIR="$OPTARG" ;;
       	esac
done

echo "The builddir is $THE_BUILDDIR"

#echo "param $1"
#DFILE=nohead-01.oleo
shift $(( OPTIND - 1 ))
#DFILE=$1.oleo
SCR=`basename "$@"` # e.g. issue19.scr
echo "SCR=$SCR"
OLEO="$SCR.oleo"
echo "OLEO=$OLEO"
IFILE="$THE_SRCDIR/$SCR"
echo "IFILE=$IFILE"
CMD="neoleo --ignore-init-file -H $TMPFILE < $IFILE"
echo "CMD=$CMD"
eval $CMD

#echo "DFILE is $DFILE"

#OUTFILE="$THE_BUILDDIR/out/$OLEO"
#echo "OUTUT=$OUTPUT"
#VERFILE="$THE_SRCDIR/verified/$OLEO"
set_productions "$@.oleo"
echo "VERFILE=$VERFILE"

mv $TMPFILE $OUTFILE
diff $OUTFILE $VERFILE
exit $?
