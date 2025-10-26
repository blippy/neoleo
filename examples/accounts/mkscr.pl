#BEGIN { $/ = "\t"; } # set field separator to tab

my %totals;

while(<>) {
	chomp $_;
	#print $_;
	my $str = $_ =~ s/\"//gr; 
	#print "str = " , $str , "\n";

	my ($x, $dr, $cr, $amount) = split("\t", $str);
	#$totals{$dr} = $totals{$dr} + $amount;
	$totals{$dr} += $amount;
	$totals{$cr} -= $amount;
	#print "acc $dr\n";
}

#print %totals;
#print $totals{"foo"};


sub tprint  {
	my ($acc, $row) = @_;
	#print $totals{$acc};
	print "set-cell $row 2 $totals{$acc}\n" if exists $totals{$acc};
}

#print $totals{"bank"};
#print %totals;
#print "totals " , %totals["\"bank\""];
tprint("bank", 23);
tprint("open", 31);
tprint("car", 12);
tprint("wages", 5);
tprint("visa", 30);
tprint("shop", 13);
print "w accts.oleo\nq\n";

