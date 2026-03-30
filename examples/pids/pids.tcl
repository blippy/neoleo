#!/bin/sh
# A Tcl comment, whose contents don't matter \
exec neoleo -t ./pids.tcl

# 2026-03-22 created
# run using neoleo -t pids.tcl
#load libploppy.so


proc from {lst n} {
	return [lrange $lst $n [expr [llength $lst] - 1]]
}

proc reload {} {
	clear-sheet
	#set pss [exec ps -ef -o ruser,pid,ppid,cmd]
	set pss [exec ps -ef]
	set row 1
	foreach line [split $pss "\n"] {
		set fields [regexp -inline -all -- {\S+} $line]
		#puts $fields
		set pid [lindex $fields 1]
		if {![string is int $pid]} {
			set pid "\"$pid\""
		}
	
		set ppid [lindex $fields 2]
		if {![string is int $ppid]} {
			set ppid "\"$ppid\""
		}
	
		set cmd [from $fields 7]
	
		set-cell $row 1 $pid
		set-cell $row 2 $ppid 
		set-cell $row 3 "\"$cmd\""
		set row [expr $row + 1]
	
	}

	set-col-precision 1 0
	set-col-precision 2 0
}

set findval ""
proc find-proc {} {
	global findval
	set output [invoke-std-form "find:" $findval]
	if {[string equal 1 [string range $output 0 0]]} {
		set findval [string range $output 1 end]
		set row [get-row-num]
		incr row
		set mrows [max-row]
		while { $row <= $mrows } {
			set cell-val [get-cell $row 3]
			if {[string first $findval ${cell-val}] >= 0} {
				go $row 3
				break
			}
			incr row
		}
	}
}

proc kill-process {} {
	set row [get-row-num]
	set pid [get-cell $row 1]
	exec kill -9 $pid
}

# find ppid
proc parent {} {
#plog a
	set ppid [string trim [get-cell [get-row-num] 2]]
	set row 2
	set mrows [max-row]
#plog a
	while { $row <= $mrows } {
		set pid [string trim [get-cell $row 1]]
		set same [string equal $ppid $pid]		
#plog "$row <$ppid> <$pid> $same"
		if {$same} {
#plog found 		
			go $row [get-col-num]
			break;
		}
		incr row
	}
}

proc show-help {} {
	set-status {? help, / find, k kill, p parent, q quit, r reload}
}

reload
show-help
bind-key ? {show-help}
bind-key / {find-proc}
bind-key k {kill-process}
bind-key p {parent}
bind-key q {definitely-quit}
bind-key r {reload}
bind-key s {set-status {foo bar}}
# display-curses
