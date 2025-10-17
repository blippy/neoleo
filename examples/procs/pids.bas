# 2025-10-17	Added

sub kill
{
	let $cmd = concat("kill -9 ", get_cell(0, 1))
	#print("cms is ", $cmd)
	system($cmd)
}

bind("k", "call kill")
