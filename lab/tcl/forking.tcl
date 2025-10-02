package require Tclx

#set fds ""
#puts $wpipe "hello"
#flush $wpipe
#puts "[gets $rpipe]"
#fconfigure $rpipe --buffering line
#fconfigure $rpipe -buffering line
#fconfigure $wpipe -buffering line
#fconfigure $rpipe -buffering blocking

pipe prp pwp ; # parent read and write pipe
pipe crp cwp ; # child read and write pipe

#fconfigure $prp -blocking 0 -buffering none
#fconfigure $pwp -blocking 1 -buffering none

fconfigure $prp -blocking 0 -buffering line
fconfigure $pwp -blocking 1 -buffering line

fconfigure $crp -blocking 0 -buffering line
fconfigure $cwp -blocking 0 -buffering line

proc send msg {
	global pwp
	#upvar $pwp pwp1
	puts $pwp "$msg"
	flush $pwp
}

proc recv {} {
	global prp
	#upvar $prp prp1
	puts [read $prp]
}


set pid [fork]
if { $pid == 0} {
	# child process
	close $prp
	close $pwp
	dup $crp stdin
	dup $cwp stdout
	close $crp
	close $cwp
	#exec <@ $min >@ $mout ./child.sh foo bar 
	set st [exec  ./child.sh ./child.sh foo bar ]
	puts "Should reach here"
	if { $st == -1 } {
		puts "Error occurred in exec"
	}
	
} elseif { $pid > 0} {
	puts "let's go"
	puts $cwp "hello there"
	#puts [read $crp]
	close $crp
	close $cwp
	#send hello
	#recv
	#send world
	#recv
	#send q
	#recv
	wait
	close $prp
	close $pwp
} else {
	puts "Fork failed"
	exit 1
}


