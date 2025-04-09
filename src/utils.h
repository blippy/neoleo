#pragma once

/*
 * Copyright (c) 1992, 1993 Free Software Foundation, Inc.
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

#include <optional>
#include <string>
#include <stdio.h>
#include "global.h"

//std::string format(const std::string& format, ...);
extern int __make_backups;
extern int __backup_by_copying;

extern void* ck_malloc (size_t);
extern void* ck_calloc (size_t);
extern void* ck_realloc (void *,size_t);
extern void ck_free (void*);
#define ck_remalloc(OLD, SIZE) \
  ((OLD) ? ck_realloc ((OLD), (SIZE)) : ck_malloc (SIZE))

extern num_t astof(char **sp);
extern int strincmp (const char *, const char *, size_t);

extern void get_usr_stats (int, char **);
extern void set_usr_stats (int, char **);
extern FILE *xopen_with_backup (char *,const char *);
extern int xclose (FILE *);
extern char *err_msg (void);
std::string pad_right(const std::string& s, int width);
std::string pad_left(const std::string& n, int width);
std::string spaces(int n);
size_t strlen(const std::string& s);
int stricmp (const char *, const char *);


// 25/4 A defer statement by Ginger Bill 
// https://www.gingerbill.org/article/2015/08/19/defer-in-cpp/
template <typename F>
struct privDefer {
	F f;
	privDefer(F f) : f(f) {}
	~privDefer() { f(); }
};

template <typename F>
privDefer<F> defer_func(F f) {
	return privDefer<F>(f);
}

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x)    DEFER_2(x, __COUNTER__)
#define defer(code)   auto DEFER_3(_defer_) = defer_func([&](){code;})

// 25/4
std::optional<int> to_int(const std::string& str);