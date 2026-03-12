CXXFLAGS = -std=c++20 $(shell pkg-config --cflags neoleo)
LDFLAGS = $(shell pkg-config --libs neoleo)
#LDFLAGS += $(shell pkg-config --libs tcl)


app : main.o
	g++ $< -o $@ $(LDFLAGS)

main.o : main.cc
	g++ -c $<  $(CXXFLAGS)

clean :
	rm -f app *.o
