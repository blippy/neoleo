#pragma once
#include <string>

void 	clear_status();
int 	get_ch ();
int 	get_ch (WINDOW *win);
void	main_command_loop_for2019();
void 	set_status (const std::string& str);
void	show_status();
