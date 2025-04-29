#!/usr/bin/env python

col = 0
maxcol = 0
row = 0
maxrow = 0

cells = {}

def field_C(fields):
	global col, maxcol, row, maxrow, cells
	for f in fields:
		if f[0] == 'c' :
			col = int(f[1:])
			maxcol = max(col, maxcol)
		elif f[0] == 'r':
			row = int(f[1:])
			maxrow = max(row, maxrow)
		elif f[0] == 'E':
			val = f[1:]
			cells[(row,col)] = val
			


f = open("simple.oleo", "r")



for line in f.readlines():
	line = line.rstrip()
	if len(line) == 0: continue 
	if line[0] == '#': continue
	fields = line.split(';')
	cmd = fields[0]
	fields = fields[1:]
	if cmd == 'E': break
	if cmd == 'C':
		field_C(fields)
		

for r in range(1, maxrow +1):
	cols = [cells.get((r,c), '') for c in range(1, maxcol + 1)]
	print(','.join(cols))
