my %totals;

while(<>) {
	chomp $_;
	#my $str = $_ =~ s/\"//gr; 
	my $str = $_;
	my ($x, $dr, $cr, $amount) = split("\t", $str);
	#$totals{$dr} = $totals{$dr} + $amount;
	$totals{$dr} += $amount;
	$totals{$cr} -= $amount;
}


sub tprint  {
	my ($acc, $row) = @_;
	print "set-cell $row 2 $totals{$acc}\n" if exists $totals{$acc};
}

tprint("cash", 24);
tprint("bank", 23);
tprint("open", 31);
tprint("car", 12);
tprint("wages", 5);
tprint("visa", 30);
tprint("shop", 13);
print "w accts.oleo\nq\n";

