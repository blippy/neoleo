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
| c     | Do a row command                     |
| m     | Activate the menu                    |
| &lt;n&gt;p | set cell precision to 'n'        |
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

