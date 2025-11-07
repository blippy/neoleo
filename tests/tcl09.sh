#!/bin/sh

NEO=`realpath ../src/neoleo`
OLEO=tcl09.oleo
rm -f out/$OLEO


$NEO -m tcl  <<- "EOF" >out/$OLEO 
NEO=`realpath ../src/neoleo`
$NEO -m h << EOF
insert-by-col
12
13.1
"hello world"
;
rc[-1] + 2
rc[-1] + 3.3
.
save-oleo-as out/tcl09.oleo
EOF

diff out/$OLEO verified/$OLEO