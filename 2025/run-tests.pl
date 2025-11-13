#!/usr/bin/env perl

use v5.30;

my $neo = "../build/neoleo";
$neo = "../neoleo";
$neo = "../src/neoleo" if -e "src/neoleo" ;
#$neo = "../src/neoleo" if -e "../src/neoleo" ;

my $ntests = 0;
my $npasses = 0;


sub update_score {
	my $ret = shift(@_);
	$ntests++;
		if($ret == 0) {
		say "PASS";
		$npasses++;
	} else {
		say "FAIL";
	} 
}

# run a script-type file
sub scr {
	my $name = shift(@_);
	my $arg = shift(@_);
	my $cmd = "cd tests ; $neo $arg -s $name.scr";
	print "Running $name: "; #  $cmd";
	my $ret = system $cmd ;
	update_score($ret);
	#say $ret; 
}

# shell script
sub bsh {
	my $name = shift(@_);
	print "Running $name: ";
	my $ret = system "cd tests ; ./$name";
	update_score($ret);

}


sub tst {
	my $name = shift(@_);
	print "Running $name: ";
	my $ret = system "cd tests; $neo -T $name >/dev/null";
	update_score($ret);
}

scr "badrc", "badrc.oleo";
bsh "basic.sh";
scr "decompile-01";
#tst "fail"; # this should ALWAYS fail
tst "fmt";
scr "floor";
scr "insert-01";
scr "issue16";
scr "issue18";
scr "issue51";
scr "nohead-01";
tst "parser2019";
tst "pass";
scr "spans", "verified/spans.scr.oleo";
tst "vals";
scr "write";

#add_test(NAME basic     COMMAND basic.sh)
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

my $ok = ($npasses eq $ntests)? "✔ " : "❌";
say "$ok $npasses/$ntests pass";
