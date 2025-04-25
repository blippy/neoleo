CXX = c++
CXXFLAGS =  -std=gnu++23 -fmodules

#SRCS = src/basic.cc src/cell.cc
#OBJS = build/basic.o build/cell.o
SRCS = $(wildcard src/*.cc)
OBJS = $(patsubst src/%.cc,build/%.o,$(wildcard src/*.cc))

foo : $(OBJS)
	echo $(S1)
	echo O1 = $(O1)

$(OBJS) : $(SRCS)

#build/basic.o : basic.cc
#	$(CXX) -c $<

build/%.o : src/%.cc gcm.cache/mod.gcm
	$(CXX) $(CXXFLAGS) -c $< -o $@

#gcm.cache/string_view.gcm : 
#	$(CXX) $(CXXFLAGS) -c -x c++-system-header string_view -o $@   # $< -o $@

gcm.cache/std.cc :
	c++ -std=c++23 -fmodules -fsearch-include-path bits/std.cc

gcm.cache/mod.gcm : src/mod.cxx # build/gcm.cache/string_view.gcm 
	$(CXX) $(CXXFLAGS)    -Wfatal-errors -c $< -o $@
	#$(CXX) $(CXXFLAGS)  -fsearch-include-path bits/std.cc  -Wfatal-errors -c $< 
	#$(CXX) $(CXXFLAGS)  -fsearch-include-path bits/string_view.tcc  -c $< 
	#$(CXX) $(CXXFLAGS)  -fsearch-include-path bits/string_view.cc  -c $< 
	
#gcm.cache/mod.gcm : src/mod.cxx # build/gcm.cache/string_view.gcm 
#	$(CXX) $(CXXFLAGS)    -c $< -o $@
