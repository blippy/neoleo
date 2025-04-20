#!/bin/sh

NEO=$1 #this is passed in by the add_test command
echo "NEO=$NEO # for neoleo exe"
O=out/ctime.oleo

$NEO -H > $O << EOF 
# see if a segfault happens with ctime()

i
ctime(1548076419)
.
t
q
EOF

diff $O verified/ctime.scr.oleo > diffs/ctime.diffs
exit $?
