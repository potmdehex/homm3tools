#ifndef __DISPATCH_ROAD_OR_RIVER_H_DEF__
#define __DISPATCH_ROAD_OR_RIVER_H_DEF__

int dispatch_river(unsigned char adjacent,
                    unsigned char *sprite,
                    unsigned char *mirr,
                    unsigned char n_type,
                    unsigned char n_sprite);

int dispatch_road(unsigned char adjacent,
                    unsigned char *sprite,
                    unsigned char *mirr,
                    unsigned char n_type,
                    unsigned char n_sprite);

#endif
