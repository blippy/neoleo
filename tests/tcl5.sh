# 2025-10-30 Added

load-oleo "sht1.oleo"

puts "max rows [max-row]"
set v [get-cell 1 2]
if {$v != 12} {
	set-exit 1
}
