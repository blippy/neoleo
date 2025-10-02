#!/bin/bash

echo "Starting child" >> /tmp/child.log

while [ "$INPUT" != "q" ]
do
	read INPUT <fifo0
	RES="child started with args $*. You said $INPUT"
	echo "$RES" >> /tmp/child.log
	echo "$RES" >fifo1
done
