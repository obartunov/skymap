#!/usr/bin/perl

$dc1=14;
$drcg1='01';

while(<>){
	chomp;
	@dc_arr=split /\ +/;
	$drcg1=$dc_arr[0];
	@dcn_arr=split(/-/,$dc_arr[1]);
	($dc1=$dcn_arr[0])=~s/dc//ig;
	$dc2=$dcn_arr[1];
	print "0c>\{(DC)(DRCG)\}<(L)<( )<(0)<(0)\{($dc1)($drcg1)\}<( )<(-)d($dc2)<( )d(:)<( )d([D)<[ -]d(80])<( )>(-)<( )#999\n\n";
}
