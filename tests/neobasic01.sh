OFILE=neobasic01.rep
neoleo -H >out/$OFILE <<EOF
# simple test of neobasic

.BAS
println(1+2)
println(10)
.XBAS

q
EOF

diff out/$OFILE verified/$OFILE
exit $?
