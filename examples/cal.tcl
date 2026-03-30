# 2026-03-30	Added. Much to do!
set icon "🗓️"

set now [clock seconds]

proc isLeap { year} {
	if {$year % 4 != 0 } { return 0  }
	if {$year % 100 == 0} {
		if {$year % 400} { return 1 }
		return 0;
	}
	return 1
}

	 
proc daysInMonth { year month} {
	if {$month == 2} {
		if { isLeap $year } { return 29 }
		return 28
	}
	if { $month == 4 || $month == 6 || $month == 9 || $month == 11 } { return 30 }
	return 31
}

proc clock-fmt {secs fmt} {
	#global now
	return [clock format $secs -format $fmt] ; # Mon ... Sun
}
	
#set day3 [clock format $now -format {%a}] ; # Mon ... Sun
set day3 [clock-fmt $now %a] ; # Mon .. Sun
set year [clock-fmt $now %Y] ; # 2026
set mon3 [clock-fmt $now %b] ; # Jan Feb ...
set monn [clock-fmt $now %N] ; # month 1 .. 12 for Jan .. Dec
set d "$year-$mon3-01"
puts "d $d monn $monn"
set fdom_secs [clock scan $d -format {%Y-%b-%d}] ; # first day of moneth as seconds
set fdom [clock-fmt $fdom_secs %w] ; # when does the 1st day of the month start? Sun=0, Sat=6
puts "fdom $fdom"
set dim [daysInMonth 2026 3]


puts "$year $mon3 day3 $day3"
puts {Sun Mon Tue Wed Thu Fri Sat} 

set day [expr 1 - $fdom]
foreach r {0 1 2 3 4 5} {
	foreach c {1 2 3 4 5 6 7} {
		if {$day <= 0 || $day > $dim} {
			puts -nonewline "   "
		} else {
			puts -nonewline [format "%3d " $day]
		}
		incr day
	}
	puts ""
}


foreach c {2 3 4 5 6 7 8} dow {Mon Tue Wed Thu Fri Sat Sun} {
	#set-col-width $c 4
	#set-cell 2 $c \"$dow\"
}
#set-cell 1 1 "$icon"
for {set c 2} {$c <= 8} {incr c} {
#	set-col-width $c 4
}
