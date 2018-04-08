# draw a pattern of cells
# It's fairly uninteresting. The Game of Life would be great

for ?r := 1 to 10
for ?c := 1 to 10
	gotorc(?r, ?c)
	cmd('edit-cell ""')
next
next
cmd('redraw-screen')
refresh()


for ?r := 1 to 10
	for ?c := 1 to 10
		gotorc(?r, ?c)
		cmd('edit-cell "*"')
	next
	cmd('redraw-screen')
	refresh()
	sleeps(0.5)
next
