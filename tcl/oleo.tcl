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



proc ::oleo::oleo-hi {} {
	puts "oleo module says 'hi'"
}

