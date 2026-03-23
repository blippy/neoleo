# 2026-03-22 created
# run using neoleo -t pids.tcl
load libploppy.so


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

set-status "pid.tcl says hi"
bind-key h {exec whiptail --msgbox hello 10 50}
display-curses
