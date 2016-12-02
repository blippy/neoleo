load ../src/.libs/libneoleo.so
start_swig_motif
puts "now manually load pivot.oleo"
source pivot-utils.tcl
mkcsv
showcsv
mkpivot

