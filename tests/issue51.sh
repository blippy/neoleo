#!/bin/sh

NEO=`realpath ../src/neoleo`
$NEO -m 0 << EOF
# 2025-04-20 Script hanging bug
I
1
2
3
EOF
