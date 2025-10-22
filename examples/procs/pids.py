#!/usr/bin/env python3
import psutil
procs = []
for p in psutil.process_iter():
	procs.append([p.pid, p.name()])


def keyer(x): return x[1].lower()
procs.sort(key = keyer)


def princ(*arg):
	print("\n".join(arg))

princ("clr-sheet") # clear spreadsheet
princ("g", "1", "1", "I") # got to R1C1 and insert rowwise
for p in procs:
	print(p[0])
	print('"' + p[1] + '"')
	print(";")
print(".")


#princ("I", '' , '', ".") # blank out last row for when we do a kill

princ("g", "1", "2")
print("cal") # left-align the process names
princ("g", "1", "1")

