/* Created by John Åkerblom 2014-08-25 */

#include "../h3m_tile.h"

#include <stdio.h> /* Debug printf */
#define DEBUG_XY(xcmp, ycmp)                                            \
        if (x == xcmp && y == ycmp)                                     \
        {                                                               \
                printf("%d:%d:%d\n",   nw_type, n_type, ne_type);       \
                printf("%d:%d:%d\n",   w_type,  c_type, e_type);        \
                printf("%d:%d:%d\n\n", sw_type, s_type, se_type);       \
                printf("%d,%d: same:0x%02X, sand:0x%02X\n",             \
                        x, y, *adjacent_same, *adjacent_sand);          \
        }

#define ADJACENT(T) \
        (0x01 * (T == nw_type) + \
        0x02 *  (T == n_type)  + \
        0x04 *  (T == ne_type) + \
        0x08 *  (T == w_type)  + \
        0x10 *  (T == e_type)  + \
        0x20 *  (T == sw_type) + \
        0x40 *  (T == s_type)  + \
        0x80 *  (T == se_type))

int analyze_terrain(const unsigned char *types,
        unsigned int size,
        unsigned int x,
        unsigned int y,
        unsigned char *type,
        unsigned char *adjacent_same,
        unsigned char *adjacent_sand)
{
        /**
         * Macros for one dimensional coordinates for all adjacent tiles. 
         * Start with x coordinate, then add as many "rows" as the value y. 
        **/
        #define nw_coord  ((x - 1) + (size * (y - 1)))
        #define n_coord   (x + (size * (y - 1)))
        #define ne_coord  ((x + 1) + (size * (y - 1)))
        #define w_coord   ((x - 1) + (size * y))
        #define e_coord   ((x + 1) + (size * y))
        #define sw_coord  ((x - 1) + (size * (y + 1)))
        #define s_coord   ((x + (size * (y + 1))))
        #define se_coord  ((x + 1) + (size * (y + 1)))

        unsigned int c_type;
        unsigned int nw_type;
        unsigned int n_type;
        unsigned int ne_type;
        unsigned int w_type;
        unsigned int e_type;
        unsigned int sw_type;
        unsigned int s_type;
        unsigned int se_type;

        /* First only retrieve center tile's type */
        c_type  = types[x + (size * y)];
        *type   = c_type;

        /* NOTE: All info needed for SAND retrieved at this point */

        /** 
         * Get terrain types for all adjacent tiles, using their
         * one dimensional coordinates. First perform appropriate ternary
         * boundary checks for each case. Default to center tile type. 
        **/
        nw_type = (y > 0 && x > 0)?                     types[nw_coord] : c_type;
        n_type  = (y > 0)?                              types[n_coord]  : c_type;
        ne_type = (x < size - 1 && y > 0)?              types[ne_coord] : c_type;
        w_type  = (x > 0)?                              types[w_coord]  : c_type;
        e_type  = (x < size - 1)?                       types[e_coord]  : c_type;
        sw_type = (x > 0 && y < size - 1)?              types[sw_coord] : c_type;
        s_type  = (y < size - 1)?                       types[s_coord]  : c_type;
        se_type = (y < size - 1 && x < size - 1)?       types[se_coord] : c_type;

        /* NOTE: All info needed for WATER retrieved at this point */

        *adjacent_sand = ADJACENT(T_SAND)|ADJACENT(T_WATER)|ADJACENT(T_ROCK);

        /* NOTE: All info needed for DIRT retrieved at this point */

        *adjacent_same = ADJACENT(c_type);

        /* DEBUG_XY(79, 95); */

        return 0;
}

