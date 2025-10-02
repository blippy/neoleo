#!/bin/env tclsh

package require fileutil
#package require mathfunc


proc tmpname {idx} {
	set r [::tcl::mathfunc::rand]
	set num [expr {round($r*1000000)}]
	return "/tmp/neo-$idx-$num"
}

set fifonam0 [tmpname 0]
exec mkfifo $fifonam0
puts "a"
set fifo0 [open $fifonam0 w+]
puts "b"


set fifonam1 [tmpname 1]
exec mkfifo $fifonam1
puts "c"


puts "about to exec"
exec /usr/local/bin/neoleo -H <@:q
$fifonam0 > $fifonam1 &
puts "done exec"


set fifo1 [open $fifonam1 r]
fconfigure $fifo1 -blocking 0 -buffering line

puts "wring q"
puts $fifo0 "q"
read $fifo1


close $fifo0
close $fifo1


