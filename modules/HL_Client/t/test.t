#!/usr/bin/perl -w
use strict;
use Test;

BEGIN { plan test=> 6 }

use Astro::HL_Client;
use Data::Dumper;

my $hlcl=Astro::HL_Client->new();
ok(ref $hlcl, 'Astro::HL_Client');

$hlcl->HL_connect("dbi:Pg:dbname=hyperleda","","");
my $res1=$hlcl->HL_designation("m31");
ok( $res1,"MESSIER031");
my $res2=$hlcl->HL_designation("n23");
ok( $res2,"NGC0023");

my $res3=$hlcl->HL_namesolver("m31");
ok( $res3->{pgc},2557);

my $res4=$hlcl->HL_obj_byname("m51");
ok ( $res4->{data}->[0]->{objname},"NGC5194");

my $res5=$hlcl->HL_obj_nearposn(202.467942,47.1945419,40.0);
ok ( scalar @{$res5},58);
my $rr=$hlcl->HL_obj_byname("p11001");

$hlcl->HL_disconnect();
