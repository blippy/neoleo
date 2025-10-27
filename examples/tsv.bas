# 2025-10-27	Added. Works

let $mr = max_row()
let $mc = max_col()

for $r = 1 to $mr {
	for $c = 1 to $mc {
		printx(get_cell($r, $c))
		if(eq($c, $mc))
			print()
		else
			printx("\t")
	}
}

