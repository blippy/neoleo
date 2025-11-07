#!/bin/sh

OLEO=issue16.oleo
rm -f out/$OLEO

NEO=`realpath ../src/neoleo`
$NEO -m tcl <<- "EOF"
# BUG#16 blank line test
insert-by-col
1

2
.

save-oleo-as out/issue16.oleo
EOF

diff out/$OLEO verified/$OLEO