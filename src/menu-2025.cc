#include <ncurses.h>
#include <unistd.h>

#include <form.h>
#include <panel.h>

#define USE_NEWT 0
#if USE_NEWT
#include <newt.h>
#endif

#include "menu-2025.h"
#include "spans.h"
#include "basic.h"
#include "io-2019.h"
#include "io-curses.h"
#include "win.h"

//import win;

using namespace std;

static bool col_width_form();





void show_menu (bool active) // FN
{
	std::string text{"m to activate menu"};
	if(active) text = "Col c | Test t";
	text = pad_right(text, COLS);

	// display menu
	int cp = 1; // color pair
	init_pair(cp, COLOR_BLACK, COLOR_CYAN);
	WINDOW *main_menu = stdscr;
	//WINDOW* main_menu = newwin(1, COLS, 0, 0);
	wmove(main_menu, 0, 0);
	wattron(main_menu, COLOR_PAIR(cp));

	wprintw(main_menu,"%s", text.c_str());
	wattroff(main_menu, COLOR_PAIR(cp));
	//refresh();
}


#if USE_NEWT
// used by test_newt
int entryFilter(newtComponent entry, void * data, int ch, int cursor) 
{
		log("entryFilter:", ch);
		if(ch == 50) *(bool*) data = true;
		//if (ch < 48 | ch > 57) return 0; // digits
		return ch;
}


void test_newt()
{
	newtInit();
	newtDrawRootText(0,0, "press any key to exit");
	newtPushHelpLine("A help line");
	newtCenteredWindow(20, 10, "my title");

	bool aborted = false;

	
	int flags = NEWT_FLAG_RETURNEXIT * 0;
	const char *resultPtr;
	newtComponent entry = newtEntry(1, 1, "12", 10, &resultPtr, flags);
	newtEntrySetFilter(entry, entryFilter, (void*) &aborted);
	newtComponent form = newtForm(NULL, NULL, 0);
	newtComponent label = newtLabel(10,10, "You said");
	newtFormAddComponents(form, entry, label, NULL);
	struct newtExitStruct exit_status;  
	//newtRunForm(form);
	newtFormRun(form,&exit_status);  
	newtLabelSetText(label, "changed");
	newtLabelSetText(label, resultPtr);
	if(aborted) newtLabelSetText(label, "aborted");
//newtRefresh();
	newtWaitForKey();
	newtFormDestroy(form);
	//newtRefresh();
	//newtWaitForKey();
	newtPopWindow();
	//newtWaitForKey();
	newtFinished();
}

#endif

void test_edit()
{
	// TODO
	win_dow win(10, 40, 10, 10);
	box(win(), 0 , 0);
	std::string text{"Type here"};
	win_edln ed(win(), 8, 3, 3, "Input:", text);
	ed.run();
	win_print(win(), 7, 3, ed.m_input);
	wrefresh(win());
	wgetch(win());
}

// shown when you hit the menu button (m key)
void process_menu() // FN
{
	show_menu(true);
	defer1 d(show_menu, false);
	
	switch(get_ch()) {
		case 'c': col_width_form(); break;
		case 't': test_edit(); break;
#if USE_NEWT
		case 't': test_newt(); break;
#endif
	}
}

//static constexpr int CTRL(int c) { return c & 037; }


// the examplar is io-2019.cc:nform_c
bool col_width_form() 
{
	string input{to_string(get_width())};
	win_dow win(7, 30, 2, 0);
	WINDOW* w = win();
	//WINDOW *w = newwin(7, 30, 2 , 0); // lines cols y x
	//defer1 d1(delwin, w);
	box(w, 0 ,0);


	
	win_edln ed(w, 2, 1, 3, "Cursor width:", input);
	ed.m_only_digits = true;
	ed.run();
	if(ed.m_cancelled) return true;
	input = ed.m_input;
	mvwprintw(w, 2, 2, "You said '%s'", input.c_str());
	//mvwprintw(w, 3, 2, "C for canel");
	wrefresh(w);
	bool accept = false;
	auto new_width = to_int(input);
	if(new_width) {
		mvwprintw(w, 3, 2, "A for accept, C for cancel");
		wrefresh(w);		
		while(1) {
			int ch = wgetch(w);
			if(ch == 'a') { accept = true; break;}
			if(ch == 'c') { break;}
		}
	} else {
		mvwprintw(w, 3, 2, "Bad input. C for cancel");
		wrefresh(w);
		while(wgetch(w) == 'c');
	
	}

	if(accept) {
		set_width(new_width.value());
	}

	//cur_io_repaint();
	return true;

}
