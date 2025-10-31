# 2025-10-30 Added Works

load-oleo "trans.oleo"
puts "hello from accum"
set mr [max-row]
puts "max row $mr"

#set totals {}

set c 2
for {set r 1} {$r <= $mr} {incr r} {
	set dr [get-cell $r 2]
	set cr [get-cell $r 3]
	set amnt [get-cell $r 4]
	if {! [info exists totals($dr)]} {
		set totals($dr) 0
	}
	set totals($dr) [expr $totals($dr) + $amnt]
	if {! [info exists totals($cr)]} {
		set totals($cr) 0
	}
	set totals($cr) [expr $totals($cr) - $amnt]
}

puts "the array"
foreach {key value} [array get totals] {
    puts "$key => $value"
}
 

load-oleo "accts-plate.oleo"
set mr [max-row]
puts "mac row $mr"
for {set r 1} {$r <= $mr} {incr r} {
	set k [get-cell $r 2]
	try {
		set tot $totals($k)
		set-cell $r 2 $tot
		puts "set with [get-cell $r 2]"
	} on error {msg} {
		if {[regexp {^[a-z]+$} $k]} {
			puts "key matches $k"
			set-cell $r 2 0.0
		}
	}
}

save-oleo "accts.oleo"
