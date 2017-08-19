#include <ncurses.h>

int main()
{
	WINDOW *w = initscr();
	noecho();
	raw();
	keypad(w, true);
	mvaddstr(10, 10, "q to quit");
	refresh();

	while(true){
		int c = getch();
		mvaddstr(12, 10, "              ");
		mvaddstr(13, 10, "              ");
		if(c == 'q') break;
		if( 'a' <= c && c <= 'z')
			mvaddch(11,10, c);
		if(c == KEY_HOME) 
			mvaddstr(12, 10, "KEY_HOME      ");
		if(c == KEY_A1) // top left of keypad (aka home)
			mvaddstr(13, 10, "KEY_A1       ");
		refresh();
	}


	delwin(w);
	endwin();
	refresh();

	return 0;
}

