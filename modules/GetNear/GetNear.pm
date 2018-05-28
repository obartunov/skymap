package SkyMap::GetNear;
use strict;
use Carp;
use IO::File;
use vars qw($VERSION @ISA $AUTOLOAD);

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

sub new {
	my ($class, $RA_center, $Dec_center, $st_maglim,
		$gal_maglim, $zoomk, $SIZEX, $suppress_unkmag_gal) = @_;
	$class = ref($class) || $class;
	
	$RA_center ||= 5.0;
	$Dec_center ||= 0.0;
	$zoomk ||= 1.0;
	$SIZEX ||= 640;
	$st_maglim ||= 7.0;
	$gal_maglim ||= 12.0;
	$suppress_unkmag_gal ||=0;

	my $err=0;
	my $self={};

	$self->{RA_center}  = $RA_center;
	$self->{Dec_center} = $Dec_center;
	$self->{zoomk} = $zoomk;
	$self->{SIZEX} = $SIZEX;
	$self->{SIZEY} = 3*$SIZEX/4;
	$self->{imzoom} = $SIZEX/1280.0;
	$self->{st_maglim} = $st_maglim;
	$self->{gal_maglim} = $gal_maglim;
	$self->{suppress_unkmag_gal} = $suppress_unkmag_gal;

	$self->{OBJ} = SkyMap::GetNear::PStruct_Init(
		$RA_center, $Dec_center, $st_maglim,
		$gal_maglim, $zoomk, $SIZEX, $suppress_unkmag_gal
	);
	return bless ($self,$class);
}

sub _clearopt {
	my $opt=shift;

	foreach my $key ( keys %$opt ) {
		delete $opt->{$key} if
			(! (defined $opt->{$key} && length $opt->{$key}) );
	}
}

sub read_rc3data {
	my ($self)=@_;
	my @res;
	my $numb=SkyMap::GetNear::Pread_rc3data(\@res);
	return \@res;
}

sub select_rc3galaxies {
	my ($self)=@_;
	my @res;

	my $status=SkyMap::GetNear::Pselect_rc3galaxies($self->{OBJ}, \@res);
	return \@res;
}

sub select_tychostars {
	my ($self)=@_;
	my @res;

	my $status=SkyMap::GetNear::Pselect_tychostars($self->{OBJ}, \@res);
	return \@res;
}

sub select_a20stars {
	my ($self)=@_;
	my @res;

	my $status=SkyMap::GetNear::Pselect_a20stars($self->{OBJ}, \@res);
	return \@res;
}

bootstrap SkyMap::GetNear $VERSION;
1;
__END__

=head1 NAME

SkyMap::GetNear - access methods for getting objects near mouse click

=head1 SYNOPSIS

 my $client=SkyMap::GetNear->new( $RA_center, $Dec_center,
	$st_maglim, $gal_maglim, $zoomk, $SIZEX, $sup_unkgal_mag );

 my $res = $client->search(Query=>"array suffix");

Name of methods correlated with type of query of skymap *check .cgi modules.

=head1 DESCRIPTION

=head1 SEE ALSO

=head1 AUTHOR

Igor Chilingarian,   chil@sai.msu.su

=head1 BUGS

There are probably numerous bugs in this module. Flame author :-)

=cut

