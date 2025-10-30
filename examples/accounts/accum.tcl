# 2025-10-30 Added


puts "hello from accum"
set mr [max-row]
puts "mac row $mr"

#set totals {}

set c 2
for {set r 1} {$r <= $mr} {incr r} {
	#puts "r $r $c"
	set dr [get-cell $r 2]
	#puts "dr  $dr"
	set cr [get-cell $r 3]
	set amnt [get-cell $r 4]
	#puts "amnt $amnt"
	#puts [expr $amnt + 0]
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
#parray totals
puts "fin"

