#!/usr/bin/perl -w
use strict;
use Test;

BEGIN { plan test=> 6 }

use SkyMap::GetNear;

#a=13.5115520216601&d=47.3574431630922&z=0.833333333333333&galmaxmag=12.00&xs=640&supunkgal=1
my $map=SkyMap::GetNear->new(13.511552,47.35744316,7.0,12.0,1.0,640,1);
ok( ref $map, 'SkyMap::GetNear' );

my $rc3data=$map->read_rc3data();
ok( scalar @{$rc3data},23023);

my $rc3sel=$map->select_rc3galaxies();
ok( scalar @{$rc3sel},2);

#print STDERR "\n";
#print STDERR "N_Objs=".scalar @{$rc3sel}."\n";
#print STDERR "RA =".@{$rc3sel}->[0]->{ra}."\n";
#print STDERR "Dec=".@{$rc3sel}->[0]->{dec}."\n";
#print STDERR "mag=".@{$rc3sel}->[0]->{mag}."\n";
#print STDERR "p_name=".@{$rc3sel}->[0]->{p_name}."\n";
#print STDERR "name2=".@{$rc3sel}->[0]->{name2}."\n";
#print STDERR "name3=".@{$rc3sel}->[0]->{name3}."\n";
#print STDERR "type_obj=".@{$rc3sel}->[0]->{type_obj}."\n";

#a=3.78078199243514&d=24.3723134702918&z=0.833333333333333&m=7&xs=640
$map={};
$map=SkyMap::GetNear->new(3.7807820,24.3723134702918,7.0,12.0,0.83333333,640,1);

my $tychosel=$map->select_tychostars();
ok( scalar @{$tychosel},8);

#print STDERR "\n";
#print STDERR "N_Objs=".scalar @{$tychosel}."\n";
#print STDERR "RA =".@{$tychosel}->[0]->{ra}."\n";
#print STDERR "Dec=".@{$tychosel}->[0]->{dec}."\n";
#print STDERR "mag=".@{$tychosel}->[0]->{mag}."\n";
#print STDERR "pmRA=".@{$tychosel}->[0]->{pm_ra}."\n";
#print STDERR "pmDec=".@{$tychosel}->[0]->{pm_dec}."\n";
#print STDERR "par=".@{$tychosel}->[0]->{par}."\n";

#a=12.0263359533568&d=53.3485621407405&z=25&m=17&xs=640
$map={};
$map=SkyMap::GetNear->new(12.026335953,53.34856214,17.0,12.0,25.0,640,1);
my $a20sel=$map->select_a20stars();
ok( scalar @{$a20sel},2);

#print STDERR "\n";
#print STDERR "N_Objs=".scalar @{$a20sel}."\n";
#print STDERR "RA =".@{$a20sel}->[0]->{ra}."\n";
#print STDERR "Dec=".@{$a20sel}->[0]->{dec}."\n";
#print STDERR "mag=".@{$a20sel}->[0]->{mag}."\n";
#print STDERR "pmRA=".@{$a20sel}->[0]->{pm_ra}."\n";
#print STDERR "pmDec=".@{$a20sel}->[0]->{pm_dec}."\n";
#print STDERR "par=".@{$a20sel}->[0]->{par}."\n";

$map={};
$map=SkyMap::GetNear->new(11.0+19.0/60.0+10.42/3600.0,58.0+37.0/60.0+48.3/3600.0,17.0,12.0,0.8333333,640,0);
my $rc3sel_2=$map->select_rc3galaxies();
ok( scalar @{$rc3sel_2},8);

#print STDERR "\n";
#print STDERR "N_Objs=".scalar @{$rc3sel_2}."\n";
