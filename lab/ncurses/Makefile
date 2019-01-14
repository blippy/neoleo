ASAN=
neg : main.cc
	g++ -std=c++17 -ggdb  -fsanitize=address $^ -o $@ -lncurses -lasan

clean :
	rm neg
