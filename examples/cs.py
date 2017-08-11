#!/usr/bin/env python

import curses
from curses.panel import *
from curses import *
import curses.textpad
import time

stdscr = curses.initscr()
stdscr.immedok(True) # automatically refresh window
curs_set(0)
stdscr.keypad(1)
mousemask(1)
start_color()

mbar_color_pair = 1
init_pair(mbar_color_pair, COLOR_WHITE, COLOR_BLUE)

#curses.noecho()
#curses.echo()


LINES, COLS = stdscr.getmaxyx()
mbar = stdscr.subwin(1, COLS, 0, 0)
#wbkgd(mbar, mbar_color_pair)
mbar.bkgd(' ',  color_pair(mbar_color_pair))
mbar.addstr("File Edit", color_pair(mbar_color_pair))
#mbar.show()
stdscr.refresh()

def a_textbox():
    begin_x = 20
    begin_y = 7
    height = 5
    width = 40
    win = curses.newwin(height, width, begin_y, begin_x)
    tb = curses.textpad.Textbox(win)
    text = tb.edit()
    curses.addstr(4,1,text.encode('utf_8'))

win1 = stdscr.subwin(10,10, 10, 10)
win1.erase()
#win1.box()
win1.border(0)
win1.addstr(3, 3, "q=quit")
pane1 = new_panel(win1)
def a_menu():
    global win1, pane1
    update_panels()
    stdscr.refresh()
    pane1.show()
    while True:
        event = stdscr.getch()
        if event == ord("q"): break
        elif event == curses.KEY_MOUSE:
            _, mx, my, _, _ = getmouse()
            y, x = stdscr.getyx()
            #y, x = win1.getyx()
            #stdscr.addstr(y, x, stdscr.instr(my, mx, 5))
            win1.addstr(4,4, str(my), 2)
            win1.refresh()
        elif event == KEY_HOME:
            flash()

    pane1.hide()

try:
    a_menu()
finally:    
    curses.endwin()

#hw = "Hello world!"
#while 1:
#    c = stdscr.getch()
#    if c == ord('p'): 
#    elif c == ord('q'): break # Exit the while()
#    elif c == curses.KEY_HOME: x = y = 0

