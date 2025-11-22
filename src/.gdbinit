# quit without confirmation
define hook-quit
    set confirm off 
end

#set pagination off
file ../build/src/neoleo
#b io-curses.cc:399
#b io_init_windows
#r -s ../tests/ctime.scr
#r -H ../build/tests/out/spans.oleo
#r -s ../tests/decompile-01.scr
#r -H -b todo.bas
#r -H -b test.bas <<< q
r -m 0 -e "puts hello"
