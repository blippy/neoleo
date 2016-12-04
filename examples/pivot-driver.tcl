load ../src/.libs/libneoleo.so
start_swig_motif
puts "now manually load pivot.oleo"
get_formula 2 2
source pivot-utils.tcl
mkcsv
showcsv
mkpivot

