#!/usr/bin/env python3

import csv
import sys

fin = sys.stdin
rdr = csv.reader(fin)

print("I")
for row in rdr:
    for cell in row:
        try: 
            v = float(cell)
        except ValueError:
            v =  '"' + cell + '"'
        print(v)
    print(";")

print(".")
print("w")
print("q")
