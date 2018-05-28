package Astro::NED_Client;
use strict;
use Carp;
use IO::File;
use vars qw($VERSION @ISA $AUTOLOAD);
use Data::Dumper;

require Exporter;
require DynaLoader;
require AutoLoader;

@ISA = qw(Exporter DynaLoader);
$VERSION=0.05;

sub AUTOLOAD {
    # This AUTOLOAD is used to 'autoload' constants from the constant()
    # XS function.  If a constant is not found then control is passed
    # to the AUTOLOAD in AutoLoader.

    my $constname;
    ($constname = $AUTOLOAD) =~ s/.*:://;
    croak "& not defined" if $constname eq 'constant';
    my $val = constant($constname, @_ ? $_[0] : 0);
    if ($! != 0) {
        if ($! =~ /Invalid/) {
            $AutoLoader::AUTOLOAD = $AUTOLOAD;
            goto &AutoLoader::AUTOLOAD;
        }
        else {
                croak "Your vendor has not defined RamSearch::Client macro $constname";
        }
    }
    no strict 'refs';
    *$AUTOLOAD = sub () { $val };
    goto &$AUTOLOAD;
}

sub NED_obj_byname {
	my ($objname)=@_;
	my $res={ stat=>0, data=>[]};
	my $op=[];
	my $stat=Astro::NED_Client::PNED_obj_byname($objname,$op);
	$res->{stat}=$stat;
	$res->{data}=$op;
	return $res;
}

bootstrap Astro::NED_Client $VERSION;
1;
__END__

=head1 NAME

Astro::NED_Client - perl wrappers for libnedcli functions

=head1 SYNOPSIS

 my $client=Astro::NED_Client::NED_obj_byname("m31");

=head1 DESCRIPTION

=head1 SEE ALSO

=head1 AUTHOR

Igor Chilingarian,   chil@sai.msu.su

=head1 BUGS

There are probably numerous bugs in this module. Flame author :-)

=cut

