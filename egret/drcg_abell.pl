#!/usr/bin/perl

$Abell=14;
$drcg1='01';

while(<>){
	chomp;
	@abell_arr=split /\ +/;
	$drcg1=$abell_arr[0];
	($Abell=$abell_arr[1])=~s/abell//ig;
	print "0c>\{(ABEL)(DRCG)\}<(L)<( )<(0)<(0)\{($Abell)($drcg1)\}<( )d(:)<( )d([D)<[ -]d(80])<( )>(-)<( )#999\n\n";
}
