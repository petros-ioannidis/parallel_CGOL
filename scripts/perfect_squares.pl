#!/usr/bin/perl
use warnings;

open($div ,">div");

for $number (1..2000){
	@div = ();
	for (4..60){
		if(int(sqrt($_)) == sqrt($_)){
			if ($number%$_ == 0){
				push(@div,$_);
			}
		}	
	}
	if(@div){
		@div2 = join(", ",@div);
		print "$number => [@div2],\n";
		print $div "$number => [@div2],\n";
	}
}
close($div);
