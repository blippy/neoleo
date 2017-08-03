#!/usr/bin/env bash
OFILE=`mktemp`
neoleo  -H $OFILE <<EOF
i
12
13.1
"hello world"
;
rc[-1] + 2
rc[-1] + 3.3
.
w
q
EOF

DFILE=nohead-01.oleo
mv $OFILE out/$DFILE
diff out/$DFILE verified/$DFILE
exit $?
