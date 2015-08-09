/* Created by John Åkerblom 2014-8-25  */

#ifndef __ANALYZE_TERRAIN_H_DEF__
#define __ANALYZE_TERRAIN_H_DEF__

int analyze_terrain(const unsigned char *types, 
                        unsigned int size, 
                        unsigned int x, 
                        unsigned int y,
                        unsigned char *type,
                        unsigned char *adjacent_same,
                        unsigned char *adjacent_sand);

#endif

