#pragma once

#include <string>

double to_double(const char* strptr, bool &ok);
long to_long(const char* strptr, bool &ok);
long to_long(const std::string& str, bool &ok);
long astol (char **ptr);
