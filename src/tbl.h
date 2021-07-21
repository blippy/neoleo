#pragma once

#include <stdio.h>
#include <string>

void tbl();
void save_csv(const std::string& filename, char sep);
void save_dsv(FILE *fpout, char sep);

