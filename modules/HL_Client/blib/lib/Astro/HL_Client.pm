package Astro::HL_Client;
use strict;
use Carp;
use IO::File;
use vars qw($VERSION @ISA $AUTOLOAD);
use DBI;
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

sub new {
	my ($class ) = @_;
        $class = ref($class) || $class;
        my $self={};
	$self->{dbh}={};
	return bless ($self,$class);
}

sub _clearopt {
	my $opt=shift;

	foreach my $key ( keys %$opt ) {
		delete $opt->{$key} if
			(! (defined $opt->{$key} && length $opt->{$key}) );
	}
}

sub HL_designation {
	my ($self,$objname)=@_;
	my $outname='';
	$objname=~y/a-z/A-Z/;
	$objname=~s/\s//g;
	my $stat=Astro::HL_Client::PHL_designation($objname,$outname);
	return $outname;
}

sub HL_connect {
	my ($self,$data_source,$username,$password)=@_;
	$self->{dbh} = DBI->connect($data_source,$username,$password,{pg_server_prepare=>0});
}

sub HL_disconnect {
	my ($self)=@_;
	$self->{dbh}->disconnect;
}

sub HL_namesolver {
	my ($self,$objname)=@_;
	my $objname1=$self->HL_designation($objname);
	my $q1="SELECT pgc,flag FROM designation WHERE design='$objname1'";
	my $sth = $self->{dbh}->prepare($q1);
	my $res = $sth->execute;
	my $data = $sth->fetchall_arrayref;
	$sth->finish;
	my ($status,$pgc,$flag);
	my $rstr=scalar @{$data};
	if ($rstr == 1) {
		$pgc=$data->[0]->[0];
		$flag=$data->[0]->[1];
		$status=(($flag == 0)||($flag ==1))? 0 : ($flag == 2)? 2: 3;
	}
	if ($rstr == 0) {
		$pgc=-1;
		$status=1;
	}
	if ($rstr > 1) {
		$pgc=[];
		for (my $i=0; $i<$rstr; $i++) {
			push @{$pgc},$data->[$i]->[0];
		}
		$status=2;
	}
	my $result={stat=>$status,pgc=>$pgc};
	return $result;	
}

sub HL_get_names {
	my ($self,$pgc)=@_;
	my $res=[];
	my $q="SELECT design,flag FROM designation ".
		"WHERE pgc=$pgc ORDER BY nc";
	my $sth=$self->{dbh}->prepare($q);
	my $rres=$sth->execute;
	while(my $rr=$sth->fetchrow_hashref()){
		push @{$res},$rr;
	}
	$sth->finish;
	return $res;
}

sub HL_obj_byname {
	my ($self,$objname)=@_;
	my $objname1=$self->HL_designation($objname);
	my $resnam=$self->HL_namesolver($objname1);
	#print STDERR Dumper($resnam)."\n";
	my $stat=$resnam->{stat};
	my $data=[];
	if($stat == 2) {
		foreach my $pgc (@{$resnam->{pgc}}) {
			my $q="SELECT pgc,objname,objtype,".
				"180.0*long(crd2000)/pi() as ra,".
				"180.0*lat(crd2000)/pi() as dec,".
				"gtype,bt,e_bt,logd25,logr25,pa FROM ".
				"hldata WHERE pgc=$pgc";
			my $sth=$self->{dbh}->prepare($q);
			my $res=$sth->execute;
			my $rr=$sth->fetchrow_hashref;
			$sth->finish;
			$rr->{names}=$self->HL_get_names($pgc);
			push @{$data},$rr
		}
	}
	if($stat == 0) {
		my $pgc=$resnam->{pgc};
		my $q="SELECT pgc,objname,objtype,".
			"180.0*long(crd2000)/pi() as ra,".
			"180.0*lat(crd2000)/pi() as dec,".
			"gtype,bt,e_bt,logd25,logr25,pa FROM ".
			"hldata WHERE pgc=$pgc";
		my $sth=$self->{dbh}->prepare($q);
		my $res=$sth->execute;
		my $rr=$sth->fetchrow_hashref;
		$sth->finish;
		$rr->{names}=$self->HL_get_names($pgc);
		push @{$data},$rr
	}
	return {stat=>$stat, data=>$data};
}

sub HL_obj_nearposn {
	my ($self,$ra,$dec,$radius)=@_;
	my $data=[];
	my $rarad=3.14159265*$ra/180.0;
	my $decrad=3.14159265*$dec/180.0;
	my $radrad=3.14159265*$radius/180.0/60.0;
	my $q="SELECT pgc,objname,objtype,gtype,".
		"180.0*long(crd2000)/pi() as ra,".
		"180.0*lat(crd2000)/pi() as dec,bt,pa,logd25,".
		"pow(10,logd25)/10.0 as asize,logr25, ".
		"pow(10,logd25)/(10.0*pow(10,logr25)) as bsize FROM hldata ".
		"WHERE crd2000 @ '<($rarad,$decrad),$radrad>'::scircle";
	my $sth=$self->{dbh}->prepare($q);
	my $res=$sth->execute;
        while(my $rr=$sth->fetchrow_hashref()){
		$rr->{names}=$self->HL_get_names($rr->{pgc});
                push @{$data},$rr;
        }
	$sth->finish;
	return $data;
}

bootstrap Astro::HL_Client $VERSION;
1;
__END__

=head1 NAME

Astro::HL_Client - perl HyperLeda client

=head1 SYNOPSIS

 my $design=Astro::HL_Client::HL_designation("m31");

=head1 DESCRIPTION

=head1 SEE ALSO

=head1 AUTHOR

Igor Chilingarian,   chil@sai.msu.su

=head1 BUGS

There are probably numerous bugs in this module. Flame author :-)

=cut

