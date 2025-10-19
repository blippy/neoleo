#pragma once
#include <string>

void 	clear_status_line();
int 	get_ch ();
int 	get_ch (WINDOW *win);
void	main_command_loop_for2019();
void 	write_status (const std::string& str);
