#!/usr/bin/env python3
import argparse
import os

def sys(cmd):
    return os.system(cmd)

def stage01():
    res = sys("cd .. && make dist && make distcheck")
    rstr = "PASS" if res == 0 else "FAIL"
    print(rstr, ": stage01 checking basic compilation")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-1", "--stage01", help="stage 01 construction", action="store_true")
    args = parser.parse_args()
    #print(args)
    if args.stage01: stage01()

if __name__ == "__main__": main()    
