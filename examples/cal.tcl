# 2026-03-30	Added. Much to do!
package require struct::matrix

set icon "🗓️"


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
		if { [isLeap $year] } { return 29 }
		return 28
	}
	if { $month == 4 || $month == 6 || $month == 9 || $month == 11 } { return 30 }
	return 31
}

set months {Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec}

proc clock-fmt {secs fmt} {
	return [clock format $secs -format $fmt] ; # Mon ... Sun
}

proc compose-cal { year month } {
	set m [::struct::matrix]
	$m add columns 7
	$m add rows 7
	$m set cell 1 1 foo
	global months
	set dim [daysInMonth $year $month] ; # num days in in month
	set fdom1  [lindex $months [expr $month -1]] ; # 1 .. 12 => Jan .. Dec
	set fdom2 [format "%4d-%3s-01" $year $fdom1]
	#puts "fdom2 $fdom2"
	set fdom_secs [clock scan $fdom2 -format {%Y-%b-%d}] ; # first day of month as seconds
	set fdom [clock-fmt $fdom_secs %w] ; # when does the 1st day of the month start? Sun=0, Sat=6
	#puts "fdom $fdom"
	puts "         $fdom1 $year" 
	set day [expr 1 - $fdom]
	$m set row 0 {Sun Mon Tue Wed Thu Fri Sat} 
	foreach r {1 2 3 4 5 6} {
		foreach c {0 1 2 3 4 5 6 } {
			if {$day > 0 && $day < $dim} {	$m set cell $c $r $day 	}
			incr day
		}
		#puts ""
	}
	#puts ""
	
	return $m
}


set now [clock seconds]
	
#set day3 [clock format $now -format {%a}] ; # Mon ... Sun
set day3 [clock-fmt $now %a] ; # Mon .. Sun
set year [clock-fmt $now %Y] ; # 2026
set mon3 [clock-fmt $now %b] ; # Jan Feb ...
set monn [clock-fmt $now %N] ; # month 1 .. 12 for Jan .. Dec
set d "$year-$mon3-01"
#puts "d $d monn $monn"

set mat [::struct::matrix]
$mat add columns 6
$mat add rows 7
$mat set cell 1 1 foo
	

proc print-cal {year month} {
	set m [compose-cal $year $month]
	puts [$m format 2string] 
} 


print-cal 2026 2

#print-cal 2026 3

#print-cal 2026 4