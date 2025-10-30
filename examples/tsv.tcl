# 2025-10-30	Added. Works

set mr [max-row]
set mc [max-col]

for {set r 1} {$r <= $mr} {incr r} {
	for {set c  1} {$c <= $mc} {incr c} {
		set v [get-cell $r $c]
		puts -nonewline "$v"
	
		if {$c == $mc} {
			puts ""
		} else {
			puts -nonewline "\t"
		}
	}
}

