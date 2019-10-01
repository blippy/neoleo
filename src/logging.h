#pragma once

#include <string>
#include <sstream>
void log_debug(const std::string& s);
void log_debug(const char* s);

template<typename... Args>
void log(Args ... args) {
	std::ostringstream ss;
	(ss << ... << args);
	log_debug(ss.str());
}
