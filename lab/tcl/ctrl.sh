#!/bin/bash


mkfifo fifo0 fifo1
#( ./parent.sh  ) &
#( ./child.sh  ) &
#( exec 30<fifo0 31<fifo1 )
./parent.sh &
./child.sh <fifo0 >fifo1

