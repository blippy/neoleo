#!/bin/sh

NEO=`realpath ../src/neoleo`
$NEO -H  badrc.oleo << EOF
# 2025-05-21 Test for bad RC in oleo file

! rm -f out/badrc.oleo

w out/badrc.oleo
EOF

diff out/badrc.oleo verified/badrc.scr.oleo
