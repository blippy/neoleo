#!/bin/sh

NEO=`realpath ../src/neoleo`
$NEO -m tcl <<- "EOF"
# 2025-04-20 Script hanging bug
insert-by-row
1
2
3
"EOF"
