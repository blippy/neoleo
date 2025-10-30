# 2025-10-30	Added
# Test get-cell

# use sht1.tcl

puts "Running tcl4.tcl"

set v [get-cell 1 2]
puts "cell(1,2) is $v"
if { $v != 12 } {
	set-exit 1
}

set v [get-cell 2 2]
puts "cel(2,2) is $v"
if { $v != "hello world" } {
	set-exit 2
}
