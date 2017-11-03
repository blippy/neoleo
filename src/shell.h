#pragma once

#include <string>

void run_shell_output_commands(int fildes);
void run_shell(char* cmd);
std::string getline_from_fildes(int fildes);

