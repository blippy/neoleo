#!/usr/bin/env python3

import argparse

col = 0
maxcol = 0
row = 0
maxrow = 0

cells = {}

def field_C(fields):
	#print("field_C calling with fields", fields)
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
			
def field_F(fields):
	global col, maxcol, row, maxrow, cells
	for f in fields:
		if f[0] == 'r':
			row = int(f[1:])
			maxrow = max(row, maxrow)



def main(path):
	global col, maxcol, row, maxrow, cells
	#global cells
	#print("file ", path)
	f = open(path, "r")
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
		if cmd == 'F':
			field_F(fields)
	#print("cells are ", cells)
		
	for r in range(1, maxrow +1):
		cols = [cells.get((r,c), '') for c in range(1, maxcol + 1)]
		print('\t'.join(cols))


if __name__ == "__main__":
	#print("calling from main")
	p = argparse.ArgumentParser(
			prog="neodsv.py",
			description = "Convert neoleo files to delimited output")
	p.add_argument('rest', nargs=argparse.REMAINDER)
	args = p.parse_args()
	#print(args)
	rest = args.rest
	if(len(rest) == 0):
		print("No files to process")
		exit(0)
	main(rest[0])
			

