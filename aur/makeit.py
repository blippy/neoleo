import os

def sys(cmd):
    return os.system(cmd)

def stage01():
    res = sys("cd .. && make dist && make distcheck")
    if res == 0:
        print "PASS", 
    else:
        print "FAIL",
    print ": stage01 checking basic compilation"            


def main():
    stage01()

if __name__ == "__main__": main()    
