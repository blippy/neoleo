BINS = app-1 app-2

CXX = g++
#CXX = clang++

CXXFLAGS = -std=c++23 -Wfatal-errors -ggdb
LIBS = -lm

all : app-1 app-2

app-% : parser-%.cc
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)


#app-1 : parser-1.cc
#	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

#app-2 : parser-2.cc
#	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

	
clean:
	rm -f $(BINS)

