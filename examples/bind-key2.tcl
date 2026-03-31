# run with neoleo -t key-bind.tcl
# Then type `t' to see the status set:
load libploppy.so
#package require struct::matrix

# bind-key t test-binding

# Type s for a more sophisticated example:
bind-key s {set-status {Hello from Tcl}} 
display-curses
