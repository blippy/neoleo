#!/bin/sh
# A Tcl comment, whose contents don't matter \
exec neoleo -t ./pids.tcl

# 2026-03-22 created
# run using neoleo -t pids.tcl
#load libploppy.so


proc from {lst n} {
	return [lrange $lst $n [expr [llength $lst] - 1]]
}

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
	set cmd "kill -9 $pid"
plog "kill-process called $row $pid $cmd"
	exec kill -9 $pid
}

set-status "pid.tcl says hi"
bind-key / {find-proc}
bind-key k {kill-process}
bind-key q {definitely-quit}
bind-key s {set-status {foo bar}}
# display-curses
