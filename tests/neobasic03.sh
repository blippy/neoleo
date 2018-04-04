OFILE=neobasic03.rep
neoleo -H  <<EOF
bload neobasic03.bas
EOF
#diff out/$OFILE verified/$OFILE
#exit $?
