use ExtUtils::MakeMaker;
use ExtUtils::Manifest qw(maniread);

WriteMakefile(
    'NAME'      => 'SkyMap::GetNear',
    'VERSION_FROM' => 'GetNear.pm', # finds $VERSION
    'INC'       => "-I../../sky/ -I../../cgic/ -I../../gd1.2c/ -I../../ClibPDF/source/",
    'OBJECT'    => "GetNear.o ../../sky/libgetnear.a",
    depend      => {
        'GetNear.o' => '../../sky/libgetnear.a'
    },
#    CCFLAGS     => '-g',
);

