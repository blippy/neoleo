Lightweight curses spreadsheet based on GNU oleo.

If you are downloading the file from github, most people, especially
distro packagers wanting to do typical configure && make && make install
should download the file neoleo-X.Y.Z.tar.gz, rather than the
source files.

## Snapshot

![snapshot](snapshot.png "snapshot")

## Quick start
-----------

^c	copy cell formula to clipboard
^g 	cancels current operation
^l	set cell alignment left
^q 	quit
^r	set cell alignment right
^s 	save document
^v	paste cell formula from clipbaord

Documents and examples
----------------------

Neoleo installs examples and html documentation in a shared directory.
The location of this directory is dependent on the `prefix' option
supplied during the package configuration. It defaults to 
`/usr/local/share/oleo'. To determine the actual location, type:
	neoleo --version
The line prefixed with `Datadir:' will give you the required information.

You should explore that directory to see what is available.

Features
--------

* Ncurses interface
* Macro language
* Sizeable library of expressions (but not statistical regressions)
* Primitive file format conversions


Compiling/installing
--------------------

Read INSTALL, which contains general instructions, plus specific 
instructions like compiling from git, as well as trouble-shooting. 
Platform-specific comments are contained in INSTALL-${platform} 
files. INSTALL-debian covers Debian-derived distros like Ubuntu
and Mint.



Technical Reports
-----------------

Technical Reports, which is mostly of interest to developers, but
may have some bearing on users, so they may be advised to read them.


Links
-----

*  bug-oleo archives: http://lists.gnu.org/archive/html/bug-oleo/
*  Freshcode announcement: http://freshcode.club/projects/neoleo
*  papoanaya has a version of oleo on github: https://github.com/papoanaya/oleo
