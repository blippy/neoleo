# SEE neot
load libneoleo.so

# since there is so much crashing, I need to be able to do a lot of
# debugging using gdb. I need to get `neot' working without crashing.
# When it works, I will basically be able to copy its functionality
# here. 
# See neoleo_swig.c:neot_test0() for a basic runnable case without
# graphics which I need to get running first.
#

# Sequence of events is cribbed from
# io-term.c:main0()
MdiInitialize
#PlotInit # skip for now.
AllocateDatabaseGlobal
InitializeGlobals
# parse_command_line # skip for now
init_basics

# the following causes crash:
swig_read_file_and_run_hooks "../examples/pivot.oleo" 0
