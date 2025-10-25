#!/bin/sh

NEO=`realpath ../src/neoleo`
$NEO -H << EOF
# 2025-04-20 Script hanging bug
I
1
2
3
EOF
