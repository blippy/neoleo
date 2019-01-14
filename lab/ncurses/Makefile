ASAN=
neg : main.cc
	g++ -std=c++17 -ggdb  -fsanitize=address $^ -o $@ -lasan -lncurses

clean :
	rm neg
