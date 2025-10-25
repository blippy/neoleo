#!/bin/sh

NEO=`realpath ../src/neoleo`
$NEO  -H verified/spans.scr.oleo << EOF
# 2025-04-19 created
# simple test of the spans functionality
# Can it simply put out what it sucked in?

! rm -f out/spans.oleo
I
.

w out/spans.oleo
EOF

diff out/spans.oleo verified/spans.scr.oleo
