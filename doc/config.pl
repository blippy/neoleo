#!/usr/bin/env perl


my @texis = qw/ neoleo.texi headless.texi/;

my $info = <<END;

all : neoleo.info html/index.html Makefile

Makefile : config.pl
	echo "Remaking Makefile"
	perl \$<

neoleo.info : @texis
	makeinfo neoleo.texi

html/index.html : neoleo.texi
	makeinfo --html neoleo.texi -o html

clean :
	-rm -rf html neoleo.info
END

open(MK, ">Makefile");
print MK $info;
close MK;
