/* Created by John Åkerblom 2014-12-29 */

#ifndef __ANALYZE_ROAD_OR_RIVER_H_DEF__
#define __ANALYZE_ROAD_OR_RIVER_H_DEF__

int analyze_road_or_river(const unsigned char *types, 
                        unsigned int size, 
                        unsigned int x, 
                        unsigned int y,
                        unsigned char *type,
                        unsigned char *adjacent);

#endif

