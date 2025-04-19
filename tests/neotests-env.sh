#!/usr/bin/env bashsh

#echo "neotests-env.sh says hi"

function set_productions()
{
	BASE=`basename $1`
	echo "BASE=$BASE"
	OUTFILE=$ABUILDDIR/out/$BASE
	echo "OUTFILE=$OUTFILE"
	VERFILE=$ASRCDIR/verified/$BASE
	echo "VERFILE=$VERFILE"
}

export -f set_productions

# This should be the location of this script (neotests-env.sh) itself
TD=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
#echo "TD=$TD # location of neotests-env.sh script"
export TD
cd $TD # We always start from the tests dir

NEO=$PWD/../src/neoleo
#echo "NEO=$NEO # the neoleo executable"
export NEO

