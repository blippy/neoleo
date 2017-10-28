#!/usr/bin/env python3
import argparse
import hashlib
import os
import re
import shutil
from string import Template

def sys(cmd):
    return os.system(cmd)

def stage01():
    res = sys("cd .. && make dist && make distcheck")
    rstr = "PASS" if res == 0 else "FAIL"
    print(rstr, ": stage01 checking basic compilation")

def stage02():
    # replacement for mksetup

    shutil.rmtree("release", ignore_errors=True)
    os.makedirs("release")
    shutil.rmtree("test", ignore_errors=True)
    os.makedirs("test")

    # get the version according to configure.ac
    txt = open("../configure.ac").read()
    # AC_INIT([neoleo], [7.0.0])
    pat = re.compile("AC_INIT\\(\\[neoleo\\], \\[(.+)\\]\\)")
    config_version = pat.search(txt).group(1)
    # TODO check that this cross-matches with everything else: NES, PKGBUILD,???

    tarball = "neoleo-"+config_version + ".tar.gz"
    md5 =hashlib.md5()
    md5.update(open("../" + tarball, mode="rb").read())
    md5 = md5.hexdigest()

    # create PKGBUILDs
    praw = Template(open("PKGBUILD.tmpl").read())
    d = dict(VERSION=config_version, MD5SUM=md5)
    d['SOURCE'] = 'https://github.com/blippy/neoleo/releases/download/v$pkgver/$pkgname-$pkgver.tar.gz'
    ptxt = praw.safe_substitute(d)
    open("release/PKGBUILD", "w").write(ptxt)
    d['SOURCE'] = "file://../../neoleo-7.0.0.tar.gz"
    ptxt = praw.safe_substitute(d)
    open("test/PKGBUILD", "w").write(ptxt)
    #print(ptxt)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-1", "--stage01", help="stage 01 construction", action="store_true")
    parser.add_argument("-2", "--stage02", help="stage 02 construction", action="store_true")
    args = parser.parse_args()
    #print(args)
    if args.stage01: stage01()
    if args.stage02: stage02()

if __name__ == "__main__": main()    
