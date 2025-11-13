#!/bin/sh

OLEO=spans.oleo
rm -f out/$OLEO


NEO=`realpath ../src/neoleo`
$NEO  -m tcl verified/$OLEO <<- "EOF"
# 2025-04-19 created
# simple test of the spans functionality
# Can it simply put out what it sucked in?

I
.

save-oleo-as out/spans.oleo

EOF

diff out/$OLEO verified/$OLEO
