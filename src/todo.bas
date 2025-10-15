# 2025-10-10 Added. Works.
# Some simple to-do commands

let $todo_db = 0
let $todo_tick = "\"✅\""
sub tick
{
	let $tick_v = get_cell(0, 3)
	let $tick_type = gct(0, 3)
	if $todo_db print("cell is ", $tick_type, $tick_v)
	if  eq(0, $tick_v)
		set_cell(0, 3, $todo_tick)
	else
		set_cell(0, 3, "")

}

bind("t", "call tick")

if $todo_db {
	#set_cell(0, 3, "\"Y\"")
	call tick
	call tick
	call tick
	call tick
	call tick
	QUIT(0)
	#set_cell(1,1, "")
}
