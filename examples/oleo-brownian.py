#!/usr/bin/env python3

# This file is supposed to be a demonstration of "brownian motion"
# in a spreadsheet using
#   M-x shell oleo-brownian.py
# Unfortunately, it does not update the spreadsheet periodically,
# so it only shows the finished result, not intermediate cells.
# So it is not possible to animate the motion.

import random, time

r = 5
c = 10

def delta():
    global r, c
    while True:
        dr = random.randint(-1, 1)
        if r+dr <1 or r+dr >20: continue
        dc = random.randint(-1, 1)
        if c+dc <1 or c+dc>9: continue
        if dr != 0 or dc != 0: break
    return r+dr, c+dc

def out(text):
    print(text, flush = True)

for n in range(10):
    r, c = delta()
    out('goto-cell r' + str(r) + 'c' + str(c))
    out('set-cell "X"')
    out('recalculate')
    out('redraw-screen')
    time.sleep(0.5)
    out('set-cell "."')


