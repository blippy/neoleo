println("Start neobasic03.bas")

#what is the meaning of life?
let ?life := 41+1
println("Meaning of life is ", ?life)

println("Testing loop 1 to 3")
for ?i := 1 to 3
	println(?i)
next

println("String tests")
println('Single quotes')
println('Embedded "double quotes" within single quotes')

def inc(?v)
	let ?v := ?v +1
	?v
fed
println("inc 12=", inc(12), "expecting 13")

if 1<2 then
	println("1 is less than 2")
else
	println("1 is not less than 2")
fi
	
def fact(?n)
	if ?n < 2 then
		let ?v := 1
	else
		let ?v :=  ?n * fact(?n-1)
	fi
	?v
fed
println("10!=", fact(10), "Expecting 3628800")

println("Test a while loop")
let ?i := 0
while ?i < 5
	println(?i)
	let ?i := ?i +1
wend
