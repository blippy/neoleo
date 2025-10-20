#pragma once

#include <string>
#include <tuple>

#include <ncursesw/ncurses.h>

void win_print(WINDOW *w, const char* str);
void win_print(const char* str);
void win_print(WINDOW *w, const std::string& str);
void win_print(const std::string& str);
void win_print(WINDOW* w, int y, int x, const std::string& str);
void win_print(int y, int x, const std::string& str);
std::tuple<int, int> win_getyx(WINDOW *win = stdscr);

//int CTRL(int c);
constexpr int CTRL(int c) { return c & 037; }


class win_dow {
	public:
		win_dow(int nlines, int ncols, int begin_y, int begin_x);
		~win_dow();
		void print_at(int y, int x, const std::string& str);
		WINDOW* operator()() const;

	
		WINDOW* m_w = 0;
		int nlines, ncols, begin_y, begin_x;
};


class win_edln {
	public:
		win_edln(WINDOW *parent, int ncols, int begin_y, int begin_x, const std::string& desc, const std::string& input);
		~win_edln();
		void run();
		//WINDOW *m_win;
		WINDOW* m_parent;
		int m_begin_y, m_off_x, m_ncols, m_at_y, m_at_x, m_begin_x, m_desc_len;
		std::string m_input;
		bool m_cancelled = false;
		int m_pos = 0;
		bool m_only_digits = false;
	private:
		void add_char(int ch);	
};
