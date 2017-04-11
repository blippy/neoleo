#pragma once

#include <string>
void log_debug(const std::string& s);

#ifdef __cplusplus
extern "C" {
#endif


void log_debug(const char* s);

#ifdef __cplusplus
}
#endif

