# Installation overview for 16.0

To compile neoleo you need:
* cmake
* ncurses developer libraries
* tcl headers

Help is avialable for the following platforms:
[INSTALL-arch](INSTALL-arch.md)
[INSTALL-debian](INSTALL-debian.md)
[INSTALL-fedora](INSTALL-fedora.md)
[INSTALL-slack](INSTALL-slack.md)


Contents
========

Trouble-shooting
Basic Installation
Compilers and Options

mkdir build
cd build
cmake  -DCMAKE_INSTALL_PREFIX=$HOME/tmp ..
make
sudo make install

Trouble-shooting
================

* error adding symbols: DSO missing from command line

To fix:
make clean
Install ncurses libraries (e.g. sudo apt install libncurses-dev)
make


* fatal error: ncurses.h: No such file or directory

On Debian-based distros:
	sudo apt install libncurses-dev


* error: ld undefined refence to `std::filesystem::__cxx11::path::...'

This is due to an older version of g++ that does not fully support
C++17 (e.g. in  a Debian Buster-like distro). Manifests in g++ 8.3.0
for example. To fix, issue the commands:

	make clean
	env LIBS=-lstdc++fs ./configure
	make


* note about "parameter passing ... changed in GCC 7.1"

This is basically saying that there was an ABI incompatiablities between 
different versions of g++.

It shouldn't cause any problems. If you're worried, then do
	make clean
and recompile. You can get rid of the pesky warnings by compiling using
a command like so:
	env CXXFLAGS='-Wno-psabi' ./configure


* warning: non-static data member initializers only available with -std=c++11 or -std=gnu++11

  Manifested on Mint 18 (issue #7) on build of v5.0.0 due to ageing compiler (g++ version 5.4.0). Fix:
	./configure "CXXFLAGS=-std=gnu++11"
  Fixed in subsequent versions.



