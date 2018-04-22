fo = open("defun.h")
inps = fo.readlines();

waxon = True
for line in inps:
    line = line.rstrip("\n ")
    if line == "#ifdef DOC_STRINGS": waxon = False
    if waxon: print(line)
    if line == "#endif": waxon = True

