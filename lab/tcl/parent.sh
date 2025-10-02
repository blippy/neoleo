#!/bin/bash


function emit () {
	echo "$1" >fifo0
	read INPUT  <fifo1
	echo "Parent recd $INPUT"
}

emit "Line 1"
emit "hell world"
emit q
#echo q >fifo0
