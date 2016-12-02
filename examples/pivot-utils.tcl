set csvfile "/tmp/neoleo.csv"

proc mkcsv {} {
	global csvfile
	set fp [open $csvfile "w"]
	set r 1
	while 1 {
		set v1 [get_formula $r 1]
		# TODO - find better way of determing number of rows
		if { $v1 == "" } break
		set v2 [get_formula $r 2]
		puts $fp "$v1,$v2"
		set  r [expr 1 + $r]
	}
	close $fp
}

proc showcsv {} {
	global csvfile
	set fp [open $csvfile r]
	set contents [read $fp]
	puts $contents
	close $fp
}

proc mkpivot {} {
	exec Rscript pivot.r
}


