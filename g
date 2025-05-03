#!/bin/sh

print_help()
{
cat <<EOF
0TEMPLATE: a template script file
Default is to cat the file to stdout
-h	this help
-m FILE	save 0TEMPLATE as FILE
EOF
}
 
do_make()
{
	cp `which 0TEMPLATE` $OPTARG
	chmod +x $OPTARG
}

while getopts "hm:" opt; do
	case ${opt} in
		h) print_help ; exit ;;
		m) do_make ; exit ;;
		?) exit 1;; # it will automatically print the problem
	esac
done

gdb -p `pidof neoleo`
