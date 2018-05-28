use ExtUtils::MakeMaker;
use ExtUtils::Manifest qw(maniread);

WriteMakefile(
    'NAME'      => 'Astro::NED_Client',
    'VERSION_FROM' => 'NED_Client.pm', # finds $VERSION
    'INC'       => "-I../../nedclient_3/ ",
    'OBJECT'    => "NED_Client.o ../../nedclient_3/lib/libnedcli.a",
    depend      => {
        'NED_Client.o' => '../../nedclient_3/lib/libnedcli.a'
    },
#    CCFLAGS     => '-g',
);

