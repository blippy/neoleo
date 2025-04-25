#!/usr/bin/env perl

use v5.30;

my $ntests = 0;
my $npasses = 0;

sub scr {
	$ntests++;
	my $name = shift(@_);
	my $cmd = "cd tests ; ../neoleo -s $name.scr";
	print "Running $name: "; #  $cmd";
	my $ret = system $cmd ;
	if($ret == 0) {
		say "PASS";
		$npasses++;
	} else {
		say "FAIL";
	} 
	#say $ret; 
}

scr "ctime";
scr "decompile-01";
scr "floor";
scr "insert-01";
scr "issue16";
scr "issue18";
scr "nohead-01";
scr "write";

#add_test(NAME basic     COMMAND basic.sh)
#add_test(NAME ctime     COMMAND ${NEO} -s ctime.scr)
#add_test(NAME decompile COMMAND ${NEO} -s decompile-01.scr)
#add_test(NAME floor     COMMAND ${NEO} -s floor.scr)
#add_test(NAME insert    COMMAND ${NEO} -s insert-01.scr)
#add_test(NAME iss16     COMMAND ${NEO} -s issue16.scr)
#add_test(NAME iss18     COMMAND ${NEO} -s issue18.scr)
#add_test(NAME iss25     COMMAND issue25.sh)
#add_test(NAME nohead    COMMAND ${NEO} -s nohead-01.scr)
#add_test(NAME fmt       COMMAND ${NEO} -T fmt)
#add_test(NAME parser    COMMAND ${NEO} -T parser2019)
#add_test(NAME pass      COMMAND ${NEO} -T pass) # a test that always passes
#add_test(NAME spans     COMMAND ${NEO} verified/spans.scr.oleo -s spans.scr)
#add_test(NAME write     COMMAND ${NEO} -s write.scr)

my $ok = ($npasses eq $ntests)? "✅" : "❌";
say "$ok $npasses/$ntests pass";
