\#!/usr/bin/env bas1
TMPFILE=`mktemp`

THE_BUILDDIR=`pwd`

while getopts "b:" opt
do
	case $opt in
		b) THE_BUILDDIR="$OPTARG" ;;
       	esac
done

echo "The builddir is $THE_BUILDDIR"

#echo "param $1"
#DFILE=nohead-01.oleo
shift $(( OPTIND - 1 ))
#DFILE=$1.oleo
SCR="$@" # e.g. issue19.scr
echo "SCR=$SCR"
OLEO="$SCR.oleo"
echo "OLEO=$OLEO"
#DFILE="$THE_BUILDDIR/$@.oleo"
neoleo --ignore-init-file -H $TMPFILE < "$SCR"
#echo "DFILE is $DFILE"

OUTPUT="$THE_BUILDDIR/out/$OLEO"
echo "OUTPUT=$OUTPUT"
mv $TMPFILE $OUTPUT
VERFILE="$THE_BUILDDIR/verified/$OLEO"
echo "VERFILE=$VERFILE"
diff $OUTPUT $VERFILE
exit $?
