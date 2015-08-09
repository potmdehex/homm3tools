/* Created by John Åkerblom 2014-08-25 */

#ifndef __GENERATE_TILES_H_DEF__
#define __GENERATE_TILES_H_DEF__

#include "crossplatform.h"

#ifdef __cplusplus
extern "C"
{
#endif

int DLL_PUBLIC gen_tile_sprites(unsigned char *terrain_types,
                                        unsigned char *road_types,
                                        unsigned char *river_types,
                                        unsigned int size,
                                        unsigned char *h3m_data);
int gen_tile_sprites(unsigned char *terrain_types,
                                        unsigned char *road_types,
                                        unsigned char *river_types,
                                        unsigned int size,
                                        unsigned char *h3m_data);

#ifdef __cplusplus
}
#endif

#endif

