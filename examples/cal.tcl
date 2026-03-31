#!/bin/sh
# A Tcl comment, whose contents don't matter \
exec neoleo -t ./cal.tcl
#load libploppy.so


# 2026-03-30	Added. Much to do!
plog "a1"
#load oleo
plog "a3"
set v 63
plog "v $v"
#parray env
plog 64
lappend auto_path /usr/share/tcltk/tcllib1.21
source /usr/share/tcltk/tcllib1.21/pkgIndex.tcl
plog "uu"
plog  "auto_path $auto_path"
plog "tcl_library $tcl_pkgPath"
#package require oleo ; # it doesn't know where this is
plog "a2"
#plog "tcl_library $tcl_library"


puts "auto_path $auto_path"
plog "a3"
package require tcllib
plot a3a
package require struct::matrix
plog "a4"

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
	global months
	set m [::struct::matrix]
	$m add columns 7
	$m add rows 8
	set dim [daysInMonth $year $month] ; # num days in in month
	#set fdom1  [lindex $months [expr $month -1]] ; # 1 .. 12 => Jan .. Dec
	set fdom2 [format "%4d-%3s-01" $year $month]
	set fdom_secs [clock scan $fdom2 -format {%Y-%b-%d}] ; # first day of month as seconds
	set fdom [clock-fmt $fdom_secs %w] ; # when does the 1st day of the month start? Sun=0, Sat=6
	#puts "         $fdom1 $year"
	$m set row 0 [list $month $year] 
	set day [expr 1 - $fdom]
	$m set row 1 {Sun Mon Tue Wed Thu Fri Sat} 
	foreach r { 2 3 4 5 6 7} {
		foreach c {0 1 2 3 4 5 6 } {
			if {$day > 0 && $day <= $dim} {	$m set cell $c $r $day 	}
			incr day
		}
	}
	
	return $m
}


proc range {n} {
	set result [list]
	for {set i 0 } { $i < $n } {incr i} {
		lappend result $i
	}
	return $result
}
	
	
proc print-cal-curses {year month} {
plog " print-cal-curses "
	set m [compose-cal $year $month]
	#puts [range [$m rows]]
	foreach r [range  [$m rows]] {
		foreach c [range  [$m columns]] {
			#plog "printing cal $r $c"
			set-cell [expr $r + 1] [expr $c +1] [$m get cell $c $r]
		}
	}
}

proc print-cal-cli {year month} {
	set m [compose-cal $year $month]
	
	puts [$m format 2string] 
	puts ""
} 

proc init-cal {} {
plog "init-cal called"
	plog "init-cal called"
	set now [clock seconds]
	#set day3 [clock format $now -format {%a}] ; # Mon ... Sun
	#set day3 [clock-fmt $now %a] ; # Mon .. Sun
	set year [clock-fmt $now %Y] ; # 2026
	set mon3 [clock-fmt $now %b] ; # Jan Feb ...
	#set monn [clock-fmt $now %N] ; # month 1 .. 12 for Jan .. Dec
	compose-cal $year $mon3
	print-cal-curses $year $mon3
}





plog "a"
bind-key r {init-cal}
bind-key r {set-state foo}
#init-cal

#display-curses
#puts [range 10]