# 2025-10-31 Added
# Test of set-cell

set-cell 2 3 "24"
set v [get-cell 2 3]
puts $v
if {$v != 24} {
	puts "Error 1"
	set-exit 1
}

set-cell 2 4 "\"hello world\""
set v [get-cell 2 4]
puts $v
if {$v != "hello world"} {
	puts "Error 2"
	set-exit 2
}

set-cell 2 5 "r2c3+1"
set v [get-cell 2 5]
puts $v
if {$v != 25} {
	puts "Error 3"
	set-exit 3
}
