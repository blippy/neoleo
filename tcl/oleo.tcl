package provide oleo 0.2

namespace eval ::oleo {
	namespace export forn get-col incra oleo-hi
}


# only load libploppy.so if necessary
if { [catch ploppy-life] } {
	load libploppy.so
}

# increment array
# For example, see 25.tcl
proc ::oleo::incra {arr key by} {
        upvar 1 $arr a
        if {[info exists a($key)]} {
                set a($key) [expr $a($key) + $by]
        } else {
                set a($key) $by
        }
}

proc ::oleo::forn {varName from to args} {
        upvar 1 $varName var
        set len [llength $args]
        if { $len == 2 } {
                set step [lindex $args 0]
                set block [lindex $args 1]
        } elseif {$len == 1} {
                set step 1
                set block [lindex $args 0]
        } else {
                error "bad number of argument in forn"
        }

        for {set var $from} { ($var<=$to && $step>0) || ($var>=$to && $step<=0)} {set var [expr $var + $step]} {
                uplevel 1 $block
        }
}


proc ::oleo::oleo-hi {} {
	puts "oleo module says 'hi'"
}


# set the cells in a column as a list
proc ::oleo::get-col {n} {
        ::oleo::forn r 1 [max-row] 1 {
                set v [get-cell $r $n]
                lappend result $v
        }       
        return $result
}       


#proc ::oleo::print-range {r0 c0 r1 c1 } {
#	forn r $r0 $r1 1 {
#		forn c $c0 $c1 1 {
#			puts -nonewline [get-cell-fmt $r $c] 
#			if {$c < $c1} { puts -nonewline " "}
#		}
#		puts ""
#	}
#}
