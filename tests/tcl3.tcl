# 2025-10-30	Added




set mr [max-row]
set mrx 3
puts "Expecting max-row $mrx, got $mr"
if { $mr != $mrx } { 
	set-exit 1
}

set mc [max-col]
set mcx 2
puts "Expecting max-row $mcx, got $mc"
if { $mc != $mcx } { 
	set-exit 2
}
