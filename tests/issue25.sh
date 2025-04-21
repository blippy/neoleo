#!/bin/sh
# tests groff functionality
# RFE#25

# produces a groff file
# You can view it via:
# groff -T ascii -t verified/issue25.rep

REP=issue25.rep

NEO=../neoleo
if [ -f ../build/neoleo ] ; then
	NEO=../build/neoleo
fi

#NEO=neoleo
$NEO -H issue25.oleo > out/$REP << EOF
tbl
EOF

diff out/$REP verified/$REP >diffs/$REP.diffs
exit $?
