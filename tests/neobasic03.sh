#!/usr/bin/env bash

# pass in option raw to print to stdout

raw=no
if [ x"$1" == xraw ]; then
       raw=yes
fi

OFILE=neobasic03.rep

function out {
	if [ $raw == yes ]; then
		cat
	else
		cat >out/$OFILE
	fi
}

function runit {
neoleo -H  <<EOI
bload neobasic03.bas
EOI
}

runit | out

if [ $raw == no ]; then
	diff out/$OFILE verified/$OFILE
	exit $?
fi
