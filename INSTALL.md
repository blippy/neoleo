# Installation overview for 16.0

I cannot guarantee this is complete. Installation is such a fiddly process, and each Linux distro is its own special snowflake. Let me know of any build failures.

## Contents

* Prerequisites
* Building
* Installing and Linking
* Uninstalling
* Trouble-shooting


## Prerequisites

To compile neoleo you need:
* cmake
* ncurses developer libraries
* swig
* tcl headers

### Arch (as at 2026-01-20)

```
sudo pacman -S swig tcl texlive-basic
```

Texlive might be more, or less, complete.

### Debian (Trixie)

```
sudo apt install autoconf build-essential pkgconf libncurses-dev texinfo tcl-dev
```

### Fedora

```
sudo dnf install ncurses-devel 
```

Other packages may be required.

## Obtaining and unpacking neoleo

### Git - development version

This method is for developers. Distro maintainers should use the released version instead.

```
git clone https://github.com/blippy/neoleo.git
cd neoleo
```

### Released version (for distro maintainers)

```
wget https://github.com/blippy/neoleo/archive/refs/tags/v16.0.tar.gz
tar xvfz v16.0.tar.gz
cd neoleo-16.0

```

## Building

```
mkdir build
cd build
cmake .. # but see below for customisation
```

By default, neoleo installs to `/usr/local`. Some developers like to install it to `$HOME/.local`. You can install it anywhere. Distro maintainers should install it to `/usr`, so will need to issue a command like:

```
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
```

Adjust according to taste.

Now build:

```
make
```

You are likely to get a faster build if you use simultaneous builds, e.g. :

```
make -j12
```

## Installing and Linking

**Don't do this step. I might need it for later, though**

Here be dragons. Installing is easy enough:

```
sudo make install
```

You need to link the neoleo library too. Sometimes it is as easy as:

```
sudo ldconfig
```

Problems arise when the library `libploppy.so` is not installed to a know directory. When you run `neoleo`, you may get the message:

```
neoleo: error while loading shared libraries: libploppy.so: cannot open shared object file: No such file or directory
```

It will not happen if you install to prefix `/usr/`, but `/usr/lib` might cause problems, depending on the distribution, as will other directories.


## Uninstalling

Use the command:

```
sudo make uninstall
```


## Trouble-shooting

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



