#ifndef __SKYC_DEF_H__
#define __SKYC_DEF_H__

#define Pi 3.1415926535897932
#define CBND 13046
#define TYC_COUNT 1058332
#define RC3_COUNT 23023
#define A20_COUNT 526280881

#define A20_TEST "/astrodata/sky_map_pro/dat/a20/zone0000.acc"
#define A20_DATA_PATH "/astrodata/sky_map_pro/dat/a20/"
#define TYCHO_INDEX "/astrodata/sky_map_pro/dat/index"
#define TYCHO_DATA "/astrodata/sky_map_pro/dat/out2.bin"
#define RC3_DATA "/astrodata/sky_map_pro/dat/rc3.bin"
#define OBJ_CAT "/usr/local/discovery/data/sky_objects"
#define CON_BND "/astrodata/sky_map_pro/dat/conbnd.bin"

#ifdef USE_HL
#include "skyc_hldef.h"
#endif

#endif
