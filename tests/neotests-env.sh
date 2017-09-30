#!/usr/bin/env bash

#echo "neotests-env.sh says hi"

function set_productions()
{
	BASE=`basename $1`
	OUTFILE=$ABUILDDIR/out/$BASE
	echo "OUTFILE=$OUTFILE"
	VERFILE=$ASRCDIR/verified/$BASE
	echo "VERFILE=$VERFILE"
}

export -f set_productions
