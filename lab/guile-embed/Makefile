# autoconf support mentioned here:
# https://www.gnu.org/software/guile/manual/html_node/Autoconf-Support.html#Autoconf-Support

# will return something like -I/usr/include/guile/2.0 -pthread :
CFLAGS=`guile-config compile`

# will return something like -lguile-2.0 -lgc :
CLINK=`guile-config link`

# guile seems to leak memory. I don't think there's much I can do about it.
# CASAN = -fsanitize=address -fno-omit-frame-pointer -lasan

example : example.cc
	g++ $(CFLAGS) $(CASAN) $^ -o $@ $(CLINK)

clean :
	rm -f example
