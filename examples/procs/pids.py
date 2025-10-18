#!/usr/bin/env python3
import psutil
procs = []
for p in psutil.process_iter():
	procs.append([p.pid, p.name()])

#print(procs)

def keyer(x): return x[1].lower()
#print(procs)
procs.sort(key = keyer)
#procs = procs1
#print(procs)

print("\n".join(["g", "1", "1", "I"])) # got to R1C1 and insert rowwise
for p in procs:
	print(p[0])
	print('"' + p[1] + '"')
	print(";")
print(".")



#print("\n".join(["g", "1", "2", "i"])) # got to R1C1 and insert columnwise
#print(";")
#for p in procs:
#	print('"', p[1], '"', sep = '')
#print(".")


print("\n".join(["g", "1", "2"]))
print("cal\n") # left-align the process names
print("\n".join(["g", "1", "1"]))
#print("visual")


