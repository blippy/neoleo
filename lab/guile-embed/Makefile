# autoconf support mentioned here:
# https://www.gnu.org/software/guile/manual/html_node/Autoconf-Support.html#Autoconf-Support

# will return something like -I/usr/include/guile/2.0 -pthread :
CFLAGS=`guile-config compile`

# will return something like -lguile-2.0 -lgc :
CLINK=`guile-config link`

example : example.cc
	g++ $(CFLAGS) $^ -o $@ $(CLINK)
