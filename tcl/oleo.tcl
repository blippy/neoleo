package provide oleo 0.1

namespace eval ::oleo {
	namespace export oleo-hi
}

proc ::oleo::oleo-hi {} {
	puts "oleo module says 'hi'"
}

