#!/bin/sh

# TODO use  the functinality provided in neotests-env.sh
#source neotests-env.sh

TMPFILE=`mktemp`
echo "ABUILDDIR=$ABUILDDIR"
echo "ASRCDIR=$ASRCDIR"
echo "SRCDIR=$SRCDIR"

THE_BUILDDIR=`pwd`
THE_SRCDIR=`pwd`


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
NEO=$ABUILDDIR/../src/neoleo
echo "NEO=$NEO"
CMD="$NEO --ignore-init-file -H $TMPFILE < $IFILE"
echo "CMD=$CMD"
eval $CMD

#echo "DFILE is $DFILE"

#OUTFILE="$THE_BUILDDIR/out/$OLEO"
#echo "OUTUT=$OUTPUT"
#VERFILE="$THE_SRCDIR/verified/$OLEO"
set_productions "$@.oleo"
echo "VERFILE=$VERFILE"

mv $TMPFILE $OUTFILE


function reduce {
	egrep -v "^O;auto" $1
}

diff <(reduce $OUTFILE) <(reduce $VERFILE)
exit $?
