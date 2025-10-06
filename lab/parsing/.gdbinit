# quit without confirmation
define hook-quit
    set confirm off 
end

set pagination off
file ./app-2
#b main
#b print_acc
#r  ../accts/25.txt
r
