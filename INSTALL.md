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

## Building and basic installation

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

Install neoleo to some root-protected area:

```
sudo make install
```

You do not need `sudo` if you install to a directory that you have permissions to write to (e.g. `$HOME/.local`).


## Linking

Here be dragons. Neoleo is a static executable, so should work fine. This repo also make available a library called `libploppy.so`, which you can load into Tcl to extend it. 


You achieve that you need to link the neoleo library too. Sometimes it is as easy as:

```
sudo ldconfig
```

Problems arise when the library `libploppy.so` is not installed to a library directory known to the Linux distro. Each distro has its own quirks as to what directories it knows about. For more information, type `info neoleo tcl`, section "Entending Tcl" for more information.





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



