/* Created by John Åkerblom 2014-08-25 */

#ifndef __DISPATCH_TERRAIN_H_DEF__
#define __DISPATCH_TERRAIN_H_DEF__

int dispatch_terrain_generic(unsigned char adjacent_same, 
                        unsigned char adjacent_sand, 
                        unsigned char *pic, 
                        unsigned char *mirr);

int dispatch_terrain_dirt(unsigned char adjacent_sand, 
                        unsigned char *pic, 
                        unsigned char *mirr);

int dispatch_terrain_water(unsigned char adjacent_same, 
                        unsigned char *pic, 
                        unsigned char *mirr);

int dispatch_terrain_rock(unsigned char adjacent_same, unsigned char *pic);
int dispatch_terrain_sand(unsigned char *pic);

#endif

