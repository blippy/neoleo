#!/usr/bin/env python3

from subprocess import PIPE, Popen

from curses import wrapper

def maing(stdscr):
    # Clear screen
    stdscr.clear()

    # This raises ZeroDivisionError when i == 10.
    for i in range(0, 10):
        v = i-10
        stdscr.addstr(i, 0, '10 divided by {} is {}'.format(v, 10/v))

    stdscr.refresh()
    stdscr.getkey()

#wrapper(maing)


cli = Popen(["neoleo", "-H"], stdin = PIPE, stdout=PIPE, bufsize=1)

def readcli(cli): 
    return cli.stdout.readline()

def writecli(cli, text):
    if len(text) ==0 or text[-1] != '\n': text += '\n'
    text1 = text.encode('utf-8')
    cli.stdin.write(text1)
    cli.stdin.flush()

#print(cli.stdout.readline())
#print(cli.stdout.readline())
print(readcli(cli))

inp = """
edit-cell 12
down-cell
edit-cell 13
view
view
bye
"""

for txt in inp.splitlines():
    print("C: ", txt)
    #atxt = txt.encode('utf-8')
    #cli.stdin.write(atxt)
    #cli.stdin.flush()
    writecli(cli, txt)
    print("S: ", readcli(cli))
    if txt == "view":
        for i in range(0, 10): 
            print("S: ", readcli(cli))
    #outs, errs = cli.communicate(input = atxt)
    #print(outs)
