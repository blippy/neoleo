#pragma once

#include <string>
#include <tuple>

#include <ncurses.h>

void win_print(WINDOW *w, const char* str);
void win_print(const char* str);
void win_print(WINDOW *w, const std::string& str);
void win_print(const std::string& str);
void win_print(WINDOW* w, int y, int x, const std::string& str);
void win_print(int y, int x, const std::string& str);
std::tuple<int, int> win_getyx(WINDOW *win = stdscr);
