# 2025-10-22	Hmmm, There's something wrong here. It's quitting prematurely.
# 2025-10-17	Added


let $tmp = tmpnam()

sub populate
{
	system("./pids.py >/tmp/blang-pids")
	headless_script_run("/tmp/blang-pids")
}

call populate

sub kill
{
	let $cmd = concat("kill -9 ", get_cell(0, 1))
	system($cmd)
	call populate
}

#sub quitly
#{
#	QUIT(0)
#}	

bind("k", "call kill")
#bind("q", "call quit")
