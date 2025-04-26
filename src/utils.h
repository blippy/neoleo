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

#include <map>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <stdio.h>
#include "global.h"
#include "errors.h"

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

//extern void get_usr_stats (int, char **);
extern void set_usr_stats (int, char **);
extern char *err_msg (void);
std::string pad_right(const std::string& s, int width);
std::string pad_left(const std::string& n, int width);
std::string spaces(int n);
size_t strlen(const std::string& s);
int stricmp (const char *, const char *);

std::optional<int> to_int(const std::string& str); // 25/4

// look up an item in a map. If not, raise an error
template<typename S, typename T>
T map_or_raise(const std::map<S, T> &m, S key, std::string msg_if_fail) // FN
{
	try {
		return m.at(key);
	} catch (std::out_of_range e) {
		//raise_error(msg_if_fail);
		throw OleoJmp(msg_if_fail);
	}
}


// perform a reverse look-up on a map
template<typename K, typename V>
K map_reverse(const std::map<K,V> &m, V val)
{
	for (auto const& [key, v] : m) {
		if(v == val)  return key;
	}

	throw OleoJmp("map_reverse failed");

}


class defer {
public:
	//defer(std::function<void>() unwind) : m_unwind{unwind} {};
	defer(std::function<void()> fn_unwind) : m_unwind{fn_unwind} {};
	~defer() {m_unwind();};
private:
	std::function<void()> m_unwind;
};


template <typename R, typename T>
class defer1 {
public:
	//defer(std::function<void>() unwind) : m_unwind{unwind} {};
	defer1(R fn_unwind, T param)  : m_unwind{fn_unwind}, m_param{param}  {};
	~defer1() { m_unwind(m_param) ; };
private:
	R m_unwind;
	T m_param;
};


