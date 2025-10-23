# Neoleo

Lightweight curses spreadsheet based on GNU oleo.

[NEWS](NEWS)

## Quick start

Absolute minimum you need to know:
* `=` to edit a cell
* `s` to save a file
* `C-q` to quit
* `C-g` cancels any edits you are making

There is plenty more to keep you amused. Type `info neoleo` to find out more.



## Documents and examples

Neoleo installs examples in a shared directory, typically
    /usr/local/share/neoleo/examples/
or
    /usr/share/neoleo/examples/


## Compiling/installing

Debian Stable is my main development box. Test compiles are sometimes made on Slackware, Mint, and a few others. As at Oct 2025, I have given particular attention to Arch Linux, where this software has some interest.

### General overview

Read INSTALL, which contains general instructions, plus specific 
instructions like compiling from git, as well as **trouble-shooting**. 
Platform-specific comments are contained in INSTALL-${platform} 
files. INSTALL-debian covers Debian-derived distros like Ubuntu
and Mint.


### Official releases

Distro maintainers and most users  should download and use the latest 
released tarball,
e.g. neoleo-X.Y.tar.gz . Others who are feeling brave can clone the
repo in the traditional fashion:
```

You then build and install in the traditional way:

```
tar xvfz neoleo-X.Y.tar.gz
cd neoleo-X.Y
./configure # specify usual GNU options if required
make & sudo make install
```

### Development version

Development versions are WIPs and are generally not recommended for
most users. User official releases instead. But if you're still
brave ...

```
git clone git@github.com:blippy/neoleo.git
cd neoleo
autoreconf -iv
```

Then it's the usual:
```
./configure # ... with any necessary options
make
sudo make install


### Uninstalling
```
sudo make uninstall
```



## Getting fancy

`Neoleo` comes with a "headless" mode, where you can interact
via stdin/stdout, a la `ed`. One command that `neoleo` has is
`tbl`, for example, which prints a sheet in `groff`-compatible form. So you
can create your spreadsheet, and create a nice PDF with it, say
by typing the following from the command line:
```
neoleo foo.oleo -H <<<tbl 2>/dev/null | groff -t -Tascii | text2pdf >foo.pdf
```

## Technical Reports

Technical Reports, which is mostly of interest to developers, but
may have some bearing on users, so they may be advised to read them.


## Links

*  [edorig](https://github.com/edorig/oleo) oleo github page
*  [freshcode](http://freshcode.club/projects/neoleo) announcement
*  [papoanaya](https://github.com/papoanaya/oleo) oleo github page

