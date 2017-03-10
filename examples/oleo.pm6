#!/usr/bin/env perl6

grammar ofg { # oleo file grammar
	token TOP { <line>+ }
	
	token line { (<cell> || <other>) \n  }

	token rest {  \N+ }

	token cell {   'C;' <col>? <row> 'K' <rest> }

	token col { 'c' <pos> ';' }

	token row { 'r' <pos> ';' }

	token pos { <[0..9]>+ }

	token other {  <-[C]> <rest>  }

}

class ofgActions {
	has Int $.col;
	has Int $.max_col=0;
	has Int $.row;
	has Int $.max_row=0;
	has %.cells;

	method cell ($/) { 
		my @coord = "$!row:$!col";
		%.cells.push: ( @coord => $<rest> );

	}
	method col ($/) { 
		$!col = $<pos>.Int;
		#if($.col > $.max_col) { 
		$!max_col = max($!col, $!max_col);
	}

	method row ($/) {
		$!row = $<pos>.Int;
		$!max_row = max($!row, $!max_row);
	}
}

#my $contents = slurp "/home/mcarter/repos/neoleo/examples/pivot.oleo";
my $contents = slurp ; # slurp stdin

#say $contents;
my $acts = ofgActions.new;
my $match1 = ofg.parse($contents, :actions($acts));

#say $acts.cells;

for 1..$acts.max_row -> $r {
	for 1..$acts.max_col -> $c {
		my $k ="$r:$c";
		my $v = $acts.cells{$k}:exists ?? $acts.cells{$k} !! "";
		print "$v";
		my $sep = $c < $acts.max_col ?? "," !! "\n";
		print $sep;
	}
}


