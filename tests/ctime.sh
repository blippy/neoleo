#!/bin/sh

NEO=$1
echo "NEO=$NEO"
O=out/ctime.oleo

$NEO -H  << EOF 
# see if a segfault happens with ctime()

i
ctime(1548076419)
.
t
q
EOF

exit 1
