package provide oleo 0.1

namespace eval ::oleo {
	namespace export incra oleo-hi
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

# nicer implementation of the for loop
# e.g. 
# forn i 1 10 1 { 
#	# for i from 1 10 step 1
#   puts "i= $i"
# }:w

proc ::oleo::forn {varName from to step block} {
        upvar 1 $varName var
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

