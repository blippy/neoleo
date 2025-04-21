# quit without confirmation
define hook-quit
    set confirm off 
end

set pagination off
file ../build/neoleo
#b io-curses.cc:399
#b process_headless_line
#b _write_file
#b headless_main
#r -s ../tests/ctime.scr
b write_widths
r -H ../build/tests/out/spans.oleo
