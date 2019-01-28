#pragma once

#include <string>

extern bool use_coloured_output;

void keyboard_test(int fildes);
void visual_mode(int fildes);
void colours(int fildes);
std::string on_red(const std::string& str);
void show_cells(int fildes);
