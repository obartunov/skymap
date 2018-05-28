#!/usr/bin/perl

open(FGIF,"/bin/ls *.gif |");

while (<FGIF>){
	chomp;
	@a=split('\.',$_);
	system "giftogd $a[0].gif $a[0].gd";
	print "giftogd $a[0].gif $a[0].gd\n";
}
close FGIF;

