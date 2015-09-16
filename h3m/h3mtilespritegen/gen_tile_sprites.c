/* Created by John Åkerblom 2014-08-25 */

#include "crossplatform.h"
#include "gen_tile_sprites.h"
#include "terrain/analyze_terrain.h"
#include "terrain/dispatch_terrain.h"
#include "road/dispatch_road.h"
#include "river/dispatch_river.h"
#include "h3m_tile.h"
#include "analyze_road_or_river.h"

#if defined _WIN32 && NO_CRT
        #include "win/advapi_rand.h"
#else
        #include <stdlib.h>
        #define advapi_rand_init(a) 
        #define advapi_rand_exit() 
#endif

#include <stdio.h> /* debug printf */
#include <string.h>
#include <stdlib.h>

int gen_tile_sprites(unsigned char *terrain_types,
                        unsigned char *road_types,
                        unsigned char *river_types,
                        unsigned int size,
                        unsigned char *h3m_data)
{
        unsigned int i = 0;
        unsigned int y = 0;
        unsigned int x = 0;
        unsigned int idx = 0;
        unsigned int prev_fail_idx = -1;
        unsigned char terrain_type = 0;
        unsigned char terrain_sprite = 0;
        unsigned char road_type = 0;
        unsigned char road_sprite = 0;
        unsigned char river_type = 0;
        unsigned char river_sprite = 0;
        unsigned char adj_same = 0;
        unsigned char adj_sand = 0;
        unsigned char adj_road = 0;
        unsigned char adj_river = 0;
        unsigned char terrain_mirr = 0;
        unsigned char road_mirr = 0;
        unsigned char river_mirr = 0;
        int fail;
        struct H3M_TILE *h3m_tile = NULL; 
        struct H3M_TILE *n_tile = NULL; 

retry:
        fail = 0;
        advapi_rand_init(2 * 2 * size * size);
        for (i = 0, x = 0, y = 0; i < size * size; ++i)
        {
                terrain_mirr = 0;
                road_mirr = 0;
                river_mirr = 0;

                analyze_terrain(terrain_types, size, x, y, &terrain_type, 
                        &adj_same, &adj_sand);
                switch(terrain_type)
                {
                case T_DIRT:
                        fail = dispatch_terrain_dirt(adj_sand, &terrain_sprite, 
                                                        &terrain_mirr);
                        break;
                case T_ROCK:
                        fail = dispatch_terrain_rock(adj_same, &terrain_sprite);
                        break;
                case T_SAND:
                        dispatch_terrain_sand(&terrain_sprite);
                        /* Sand cannot fail since it is independent */
                        goto past_fail_check;
                case T_WATER:
                        fail = dispatch_terrain_water(adj_same, &terrain_sprite, 
                                                        &terrain_mirr);
                        break;
                default:
                        fail = dispatch_terrain_generic(adj_same, adj_sand, 
                                                        &terrain_sprite, 
                                                        &terrain_mirr);
                        break;
                }

                /**
                 * If terrain determination failed, set this tile to dirt 
                 * and re-consider the entire map as this can have a 
                 * chain-reaction on other tiles. If we already failed on
                 * it previously, set it to sand instead. We can never fail
                 * on sand. 
                **/
                if (fail)
                {
                        /*printf("fail@%d,%d\n", x, y);*/
                        idx = (size * y) + x;
                        terrain_types[idx] = (idx == prev_fail_idx)? 
                             T_SAND : T_DIRT;
                        prev_fail_idx = idx;
                        advapi_rand_exit();
                        goto retry;
                }
past_fail_check:
                n_tile = (struct H3M_TILE *)(h3m_data + ((size * (y - 1)) + x)
                        * sizeof(*h3m_tile));
                analyze_road_or_river(road_types, size, x, y, &road_type, &adj_road);
                if (0 == road_type)
                {
                        goto past_road;
                }

                fail = dispatch_road(adj_road, &road_sprite, &road_mirr, 
                                        n_tile->road_type, n_tile->road_sprite);
                if (fail)
                {
                        idx = (size * y) + x;
                        road_types[idx] = 0;
                        advapi_rand_exit();
                        goto retry;
                }
past_road:
                analyze_road_or_river(river_types, size, x, y, &river_type, &adj_river);
                if (0 == river_type)
                {
                        goto past_river;
                }
                fail = dispatch_river(adj_river, &river_sprite, &river_mirr, 
                                        n_tile->river_type, n_tile->river_sprite);
                if (fail)
                {
                        idx = (size * y) + x;
                        river_types[idx] = 0;
                        advapi_rand_exit();
                        goto retry;
                }
past_river:
                h3m_tile = 
                    (struct H3M_TILE *)(h3m_data + i * sizeof(*h3m_tile));
                h3m_tile->terrain_type = terrain_type;
                h3m_tile->terrain_sprite = terrain_sprite;
                h3m_tile->road_type = road_type;
                h3m_tile->road_sprite = road_sprite; 
                h3m_tile->river_type = river_type;
                h3m_tile->river_sprite = river_sprite;
                h3m_tile->mirroring = terrain_mirr | road_mirr | river_mirr;

                if (++x >= size)
                {
                        ++y;
                        x = 0;
                }
        }

        #if defined _WIN32 && NO_CRT
                advapi_rand_exit();
        #endif

        return 0;
}
