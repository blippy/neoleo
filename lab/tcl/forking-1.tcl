set x [open "|./child.sh myarg" r+]
fconfigure $x -buffering line -blocking 1 -translation lf

puts $x "line 1"
puts $x "line 2"
#gets $x
puts "Will we get a response?"
#gets $x
puts [read $x]
#puts [read $x]
puts "Let's close up shop"
close $x
#puts $r
