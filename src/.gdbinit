# quit without confirmation
define hook-quit
    set confirm off 
end

set pagination off
file neoleo
#b io-curses.cc:399
#b process_headless_line
#b _write_file
b headless_main
r -H < ../tests/ctime.scr
