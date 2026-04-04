package provide oleo 0.2

namespace eval ::oleo {
	namespace export AndMatcher asRows  decrd forn get-col incra incrd oleo-hi ladd mand mstreq streq toMat
	#namespace exprt ColMatcher subRows
}


package require struct::matrix

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




proc ::oleo::ladd {l} {::tcl::mathop::+ {*}$l}


# convert a spreadsheet into a Tcl matrix
proc ::oleo::toMat {} {
        set m [::struct::matrix]
        $m add rows [max-row]
        $m add columns [max-col]
        forn r 1 [max-row] 1 {
                forn c 1 [max-col] 1 {
                        set val [get-cell $r $c]
                        $m set cell [expr $c -1 ] [expr $r -1] [get-cell $r $c]
                }
        }
        return $m
}



proc ::oleo::asRows {mat} {
        set result {}
        forn r 0 [expr [$mat rows] -1] {
                lappend result [$mat get row $r]
        }
        return $result
}





# increase variable by an amount (works with doubles)
proc ::oleo::incrd {var {by 1}} {
        upvar 1 $var v
        set v [expr $v + $by]
        return $v
}

# decrease variable by an amount (works with doubles)
proc ::oleo::decrd {var {by 1}} {
        upvar 1 $var v
        set v [expr $v - $by]
        return $v
}


proc ::oleo::streq {str1 str2} { return [string equal $str1 $str2] }


oo::class create ::oleo::ColMatcher {
        variable m_func m_col m_want

        constructor {func col find} {
                set m_func $func
                set m_col  $col
                incr m_col -1
                set m_want $find
        }

        method match {row} {
                set got [lindex $row $m_col]
                #puts "colclose match want $m_want got $got row $row"
                return [$m_func $m_want $got]
        }
}


oo::class create ::oleo::AndMatcher {
        variable m_matcher1 m_matcher2

        constructor {matcher1 matcher2} {
                set m_matcher1 $matcher1
                set m_matcher2 $matcher2
        }

        method match {row} {
                set m1 [$m_matcher1 match $row]
                set m2 [$m_matcher2 match $row]
                set m  [expr $m1 && $m2]
                #puts "and match m1 $m2 m2 $m2  m $m"
                return $m
        }
}

proc ::oleo::subRows {mat matcher} {
        set result [::struct::matrix]
        $result add columns [$mat columns]
        foreach row [::oleo::asRows $mat] {
                if {[$matcher $row]} {
                        $result add row $row
                }
        }
        return $result
}

proc ::oleo::mstreq {col want} { return [::oleo::ColMatcher new ::oleo::streq $col $want]}

proc ::oleo::mand {matcher1 matcher2} { return [::oleo::AndMatcher new $matcher1 $matcher2] }

