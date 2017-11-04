#!/usr/bin/env python3
import argparse
import hashlib
import os
import re
import shutil
from string import Template
from importlib import reload

def config_version():
    # get the version according to configure.ac
    txt = open("../configure.ac").read()
    # AC_INIT([neoleo], [7.0.0])
    pat = re.compile("AC_INIT\\(\\[neoleo\\], \\[(.+)\\]\\)")
    return pat.search(txt).group(1)

def doc_version():
    lines = open("../docs/index.html").readlines()
    pat = re.compile("^Version: (\\S+)")
    result = "UNK"
    for line in lines:
        m = pat.search(line)
        if m is not None:
            result = m.group(1)
            break
    return result


def news_info():
    fp = open("../NEWS")
    fp.readline()
    version_text = fp.readline()
    version = re.compile("^VERSION:\\s+(\\S+)").search(version_text).group(1)
    status_text = fp.readline()
    status = re.compile("^STATUS:\\s+(.+)").search(status_text).group(1)
    dstamp_text = fp.readline()
    dstamp = re.compile("^DATE:\\s+(.+)").search(dstamp_text).group(1)
    return version, status, dstamp

def sys(cmd):
    return os.system(cmd)

def stage01():
    sys("cd .. && rm *.gz")

    # cross-check versioning info
    config_ver = config_version()
    print("Config version:", config_ver)
    doc_ver = doc_version()
    print("Doc version:", doc_ver)
    if(config_ver != doc_ver):
        raise SystemExit("Versions in configure.ac and index.html mismatch")
    news_ver, news_status, news_dstamp = news_info()
    if(config_ver != news_ver):
        raise SystemExit("Versions in configure.ac and NEWS mismatch")
    print("Versioning info seems reasonable")

    res = sys("cd .. && make dist && make distcheck")
    rstr = "PASS" if res == 0 else "FAIL"
    print(rstr, ": stage01 checking basic compilation")
    print("Check that the following makes sense:")
    print("news status line:", news_status)
    print("news date:", news_dstamp)

def stage02():
    # replacement for mksetup

    shutil.rmtree("release", ignore_errors=True)
    os.makedirs("release")
    shutil.rmtree("test", ignore_errors=True)
    os.makedirs("test")

    conf_version = config_version()
    tarball = "neoleo-"+conf_version + ".tar.gz"
    md5 =hashlib.md5()
    md5.update(open("../" + tarball, mode="rb").read())
    md5 = md5.hexdigest()

    # create PKGBUILDs
    praw = Template(open("PKGBUILD.tmpl").read())
    d = dict(VERSION=conf_version, MD5SUM=md5)
    d['SOURCE'] = 'https://github.com/blippy/neoleo/releases/download/v$pkgver/$pkgname-$pkgver.tar.gz'
    ptxt = praw.safe_substitute(d)
    open("release/PKGBUILD", "w").write(ptxt)
    d['SOURCE'] = "file://../../neoleo-7.0.0.tar.gz"
    ptxt = praw.safe_substitute(d)
    open("test/PKGBUILD", "w").write(ptxt)
    #print(ptxt)

def tests():
    print("config version:", config_version())
    print("doc version:   ", doc_version())
    print("NEWS:") ; news_info()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-1", "--stage01", help="stage 01 construction", action="store_true")
    parser.add_argument("-2", "--stage02", help="stage 02 construction", action="store_true")
    parser.add_argument("-T", "--tests",   help="run development tests", action="store_true")
    args = parser.parse_args()
    #print(args)
    if args.stage01: stage01()
    if args.stage02: stage02()
    if args.tests:   tests()

if __name__ == "__main__": main()    
