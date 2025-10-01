package require Tclx

#set fds ""
#puts $wpipe "hello"
#flush $wpipe
#puts "[gets $rpipe]"
#fconfigure $rpipe --buffering line
#fconfigure $rpipe -buffering line
#fconfigure $wpipe -buffering line
#fconfigure $rpipe -buffering blocking

pipe mout min
set pid [fork]
if { $pid == 0} {
	set cpid [pid]
	puts "I am child process with PID $cpid"
	#pipe rpipe wpipe
	#dup $rpipe stdin
	#dup $wpipe stdout
	exec <@ $min >@ $mout ./child.sh foo bar 
	
} elseif { $pid > 0} {
	set ppid [pid]
	puts "I am the parent process with PID $ppid"
	puts $min "First line"
	flush $min 
	#puts [gets $min]
	wait
} else {
	puts "Fork failed"
	exit 1
}


