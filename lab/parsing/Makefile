BINS = app-1 app-2

CXXFLAGS = -std=c++23 -Wfatal-errors -ggdb -lm

all : app-1 app-2

app-1 : parser-1.cc
	c++ $(CXXFLAGS) $^ -o $@

app-2 : parser-2.cc
	c++ $(CXXFLAGS) $^ -o $@

	
clean:
	rm -f $(BINS)

