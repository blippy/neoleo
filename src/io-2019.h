#pragma once
#include <string>

#include <ncurses.h>

void 	clear_status_line();
int 	get_ch ();
int 	get_ch (WINDOW *win);
void	main_command_loop_for2019();
void 	write_status (const std::string& str);
