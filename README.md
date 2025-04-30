# Neoleo

Lightweight curses spreadsheet based on GNU oleo.

[NEWS](NEWS)

## Quick start

Keybindings work in the style of vim.


Here, `C-` means the `Control character`, `M-` is the mod key, which is
either `Alt` or `ESC`.

| Key   | Purpose                              |
| ----- | ------------------------------------ |
| %     | Toggle percentage mode               |
| =     | Edit a cell                          |
| m     | Activate the menu                    |
| r     | Do a row command                     |
| C-c   | copy cell formula to clipboard       |
| C-g   | cancels current operation            |
| C-l   | set cell alignment left              |
| C-q   | quit                                 |
| C-r   | set cell alignment right             |
| C-s   | save document                        |
| C-v   | paste cell formula from clipboard    |

Up, down, left right and delete keys should work as expected.

For further keybindings, see the `man neoleo-keys` page. 
You should definitely read that manpage!


## Documents and examples

Neoleo installs examples in a shared directory, typically


## Compiling/installing

### Official releases

Distro packagers should download and use the latest released tarball,
e.g. neoleo-X.Y.tar.gz . Others who are feeling brave can clone the
repo in the traditional fashion:
```

### Development version

git clone git@github.com:blippy/neoleo.git
```

Due to the complexity of building and the integration of C++ modules,
a simple Makefile is use. A simple build:

```
make -f Makefile.neo
```

The default install directory is /usr/local. Distro maintainers will likely
want to put it in `/usr` which you can do like so:

```
PREFIX=/usr sudo make -f Makefile.neo install
```


Read INSTALL, which contains general instructions, plus specific 
instructions like compiling from git, as well as **trouble-shooting**. 
Platform-specific comments are contained in INSTALL-${platform} 
files. INSTALL-debian covers Debian-derived distros like Ubuntu
and Mint.


## Getting fancy

`Neoleo` comes with a "headerless" mode, where you can interact
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

