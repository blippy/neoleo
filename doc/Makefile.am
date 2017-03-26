info_TEXINFOS = neoleo.texi

TEXIS=		basics.texi bugs.texi cell-refs.texi entering-data.texi \
		examples.texi expressions.texi forth.texi functions.texi \
		interface.texi keymaps.texi \
		macros.texi making-changes.texi options.texi print-plot.texi \
		programs.texi ranlib.texi style.texi texinfo.tex windows.texi \
		motif.texi dbms.texi

#HTMLS=		_002eoleorc-file.html              Function-Index.html                       Other-Window.html
#		a0-mode.html                       Functions.html                            Output-Type.html
#		Advantages.html                    General-math-functions.html               Printing-and-Plotting.html
#		Alignment.html                     Getting-Help.html                         Programs.html
#		Alternatives.html                  Graphing.html                             Protection.html
#		Appearance.html                    Gsl-functions.html                        Recalculation.html
#Basic-Kbd-Macro.html               Hidden-Rows-and-Columns.html              Regions-and-Variables.html
#Basics.html                        How-to-enter-data.html                    Reporting-Bugs.html
#Boolean-functions.html             index.html                                Running-Shell-Commands.html
#Business-functions.html            Infix-expressions.html                    Save-Kbd-Macro.html
#Calling-Forth.html                 Input-Editing.html                        Saving-and-reading.html
#Cell-Height.html                   Insertion.html                            Scanning.html
#Cell-Motion.html                   Keyboard-Macros.html                      Screen-Moving.html
#Cell-referencing.html              Key-Index.html                            Search-functions.html
#Change-Window.html                 Keymaps.html                              Single-Cell-Movement.html
#Clearing-Plots.html                Large-Scale-Movement.html                 Split-Window.html
#Command-Index.html                 LessTif.html                              Statistical-functions.html
#Command-Line-Options.html          Long-One-Liners.html                      Status-Line.html
#Commands-in-Programs.html          Macros.html                               String-functions.html
#Comparison.html                    Making-changes.html                       Structural-functions.html
#Concept-Index.html                 Math-functions.html                       Style.html
#Copying.html                       Minibuffer.html                           Style-Options.html
#Cursor-and-Mark.html               Motif-Buttons-in-a-Spreadsheet-Cell.html  The-axes.html
#Database-Access.html               Motif.html                                The-Screen.html
#Database-Access-Introduction.html  Motif-Introduction.html                   Trigonometric-functions.html
#Data-sets.html                     Mouse.html                                Tutorial.html
#Date-functions.html                Movement-examples.html                    Typing.html
#Deleting.html                      Movement.html                             Using-Help.html
#Entering.html                      Moving.html                               Using-the-keyboard-shortcuts.html
#Error-values.html                  Multiple-Windows.html                     Using-the-mouse.html
#Examples-and-Utilities.html        MySQL.html                                Viewing-Choices.html
#Examples.html                      Name-Options.html                         What-you-need.html
#Expressions.html                   New-at-functions.html                     Width-of-Cells.html
#Extending-Oleo.html                noa0-mode.html                            Window-Options.html
#Fallback.html                      Oleo-values.html                          Windows.html
#Fonts-_0028in-X11_0029.html        Oleo-words.html                           Work-Area.html
#Formats-for-numbers.html           One-Line-Programs.html                    Writing-Files.html
#Forth.html                         Options.html                              Writing-programs.html
#Forth-Interpreter.html             Options-Index.html                        Xbase.html
#Forth-Word-Index.html              Other-Options.html

neoleo_TEXINFOS = $(TEXIS)
#MAIN_HTML	= neoleo/index.html
MAIN_HTML	= neoleo.html

#neoleo_DATA	= neoleo.html
#html:		$(MAIN_HTML)
EXTRA_DIST=	$(TEXIS) atlast2.html oleo_web.html oleo1.png 
noinst_DATA =	oleo_web.html $(TEXIS)

neoleo.info:	neoleo.texi $(TEXIS)


htmldir=	$(prefix)/share/neoleo
#html_DATA=	$(MAIN_HTML)	
#html_DATA	= neoleo/index.html neoleo/*.html
html_DATA	= neoleo.html

$(MAIN_HTML):	$(srcdir)/neoleo.texi ${TEXIS}
		texi2html  $(srcdir)/neoleo.texi
		
#$(MAKEINFOHTML)  $(srcdir)/neoleo.texi
		
#texi2html --subdir=. -split_chapter $(srcdir)/neoleo.texi

#$(MAKEINFOHTML) $(srcdir)/neoleo.texi


#CLEANFILES = neoleo.info
#DISTCLEANFILES = $(html_DATA)
#MAINTAINERCLEANFILES = Makefile.in

