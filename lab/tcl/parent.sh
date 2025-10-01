#!/bin/sh


mkfifo /tmp/in
mkfifo /tmp/iut

:q

echo "Starting child" >> /tmp/child.log

while [ "$INPUT" != "q" ]
do
	read INPUT
	RES="child started with args $*. You said $INPUT"
	echo $RES >> /tmp/child.log
	echo $RES
done
