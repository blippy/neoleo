# 2025-10-27	Added

#print(max_row())
#print(max_col())

let $mr = max_row()
let $mc = max_col()

let $r = 1
while (lte($r, $mr)) {
	let $c = 1
	while (lte($c, $mc)) {
		printx(get_cell($r, $c))
		if(eq($c, $mc))
			print()
		else
			printx("\t")
		let $c = ($c+1)
	}
	let $r = ($r + 1)
}

