import os

def sys(cmd):
    return os.system(cmd)

def stage01():
    res = sys("cd .. && make dist && make distcheck")
    print res

def main():
    stage01()

if __name__ == "__main__": main()    
