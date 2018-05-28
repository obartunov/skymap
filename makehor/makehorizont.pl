#!/usr/bin/perl

$fname=$ARGV[0];

@fnam=split /\./, $fname;

print "$fnam[0] $fnam[1]\n";

for ($i=0; $i<4; $i++)
{
   $az=$i*90; 
   print $fnam[0]."_a".$az."_h0_s\[SIZE\].gif\n"; 
   system "makehor $fname $az 0 800 >".$fnam[0]."_a".$az."_h0_s800.gif";
   system "makehor $fname $az 0 1024 >".$fnam[0]."_a".$az."_h0_s1024.gif";
   system "makehor $fname $az 0 1152 >".$fnam[0]."_a".$az."_h0_s1152.gif";
   system "makehor $fname $az 0 1280 >".$fnam[0]."_a".$az."_h0_s1280.gif";
   system "makehor $fname $az 0 1600 >".$fnam[0]."_a".$az."_h0_s1600.gif";
#   system "makehor $fname $az 0 2560 >".$fnam[0]."_a".$az."_h0_s2560.gif";
#   system "makehor $fname $az 0 3200 >".$fnam[0]."_a".$az."_h0_s3200.gif";
}

system "makehor $fname 0 90 800 >$fnam[0]_a0_h90_s800.gif";
system "makehor $fname 0 90 1024 >$fnam[0]_a0_h90_s1024.gif";
system "makehor $fname 0 90 1152 >$fnam[0]_a0_h90_s1152.gif";
system "makehor $fname 0 90 1280 >$fnam[0]_a0_h90_s1280.gif";
system "makehor $fname 0 90 1600 >$fnam[0]_a0_h90_s1600.gif";
#system "makehor $fname 0 90 2560 >$fnam[0]_a0_h90_s2560.gif";
#system "makehor $fname 0 90 3200 >$fnam[0]_a0_h90_s3200.gif";
