#pragma once

void main_command_loop_for2019();

void io_error_msg2019_str(const std::string& str);
void clear_status_line();

/*
template<typename ... Args>
void io_error_msg2019(const char* format, Args ... args)
{
	const std::string str = string_format(format, args...);
	io_error_msg2019_str(str);
}
*/
