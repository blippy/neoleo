#!/bin/sh


OLEO=insert-01.oleo
OUT=out/$OLEO
rm $OUT

NEO=`realpath ../src/neoleo`
$NEO -m tcl  <<- "EOF"
insert-by-col
12
13.1
sum(r1:[-1]c1)
.
go 1 1
insert-row
go 1 1
insert-by-col
42
.
save-oleo-as out/insert-01.oleo
EOF

diff $OUT verified/$OLEO
