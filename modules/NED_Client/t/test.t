#!/usr/bin/perl -w
use strict;
use Test;

BEGIN { plan test=> 3 }

use Astro::NED_Client;

my $res1=Astro::NED_Client::NED_obj_byname("m315");
ok( $res1->{stat},-1);

my $res2=Astro::NED_Client::NED_obj_byname("m31");
ok( $res2->{stat},0);
ok( $res2->{data}->[0]->{cp}->[0]->{objname},"MESSIER 031                   ");

