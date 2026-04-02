#pragma once

/*
 * $Id: oleofile.h,v 1.4 2000/08/10 21:02:51 danny Exp $
 *
 * Copyright � 1992, 1993 Free Software Foundation, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <sstream>
#include <string>
#include "wsht.hh"


using olfis_t = std::istringstream;
using olfos_t = std::ostringstream;

std::string FileGetCurrentFileName();
void 		FileSetCurrentFileName(const char *s);
void 		FileSetCurrentFileName(const std::string& s);
int			oleo_read_file (const std::string& path);
int			oleo_set_options (int, char *);
void 		oleo_write_file (wsht& wsh);
void		oleo_write_file (wsht& wsh, olfos_t&);
void 		oleo_write_file_as(wsht& wsh, std::string path);
void 		oleo_read_file (std::istream& is);
