#!/bin/sh

OLEO=badrc.oleo
rm -f out/$OLEO


NEO=`realpath ../src/neoleo`
$NEO -m tcl  <<- "EOF"
# 2025-05-21 Test for bad RC in oleo file

load-oleo badrc,oleo
save-oleo-as out/badrc.oleo
"EOF"

diff out/$OLEO verified/$OLEO
