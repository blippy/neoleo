load ../src/.libs/libneoleo.so
start_swig_motif
puts "now manually load pivot.oleo"
source pivot-utils.tcl
get_formula 2 2
mkcsv
showcsv
mkpivot

