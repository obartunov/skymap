use ExtUtils::MakeMaker;
use ExtUtils::Manifest qw(maniread);

WriteMakefile(
    'NAME'      => 'Astro::HL_Client',
    'VERSION_FROM' => 'HL_Client.pm', # finds $VERSION
    'INC'       => "-I../../egret/ ",
    'OBJECT'    => "HL_Client.o ../../egret/libegret.a",
    depend      => {
        'HL_Client.o' => '../../egret/libegret.a'
    },
#    CCFLAGS     => '-g',
);

