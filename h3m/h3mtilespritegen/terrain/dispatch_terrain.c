/* Created by John Åkerblom 2014-08-25 */

#include "border_dirt.h"
#include "border_generic.h"
#include "border_special_sand.h"
#include "border_special_sanddirt.h"
#include "border_rock.h"
#include "border_water.h"
#include "borderless.h"
#include "../h3m_tile.h"

/**
 * So aside from ideally not having 2 billion magic numbers here (could just
 * "OR together" direction bits...) and not necessarily having hundreds
 * of lines of mostly the same case checks repeated several times it might be 
 * possible to just autogenerate most cases as well. Oh well, this remains 
 * written in true retard reverse engineering-style for now.
**/

int dispatch_terrain_generic(unsigned char adjacent_same, 
                        unsigned char adjacent_sand, 
                        unsigned char *pic, 
                        unsigned char *mirr)
{
        switch (adjacent_same)
        {
        case 0xD6:
        case 0xD7:
        case 0xF6:
        case 0xF7:
                switch (adjacent_sand)
                {
                case 0x01:
                        border_w_horizontal_special_sand_n(pic, mirr);
                        break;
                case 0x20:
                        border_w_horizontal_special_sand_s(pic, mirr);
                        break;
                case 0x00:
                        border_w_horizontal_generic(pic, mirr);
                        break;
                default:
                        border_w_horizontal_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0x6B:
        case 0x6F:
        case 0xEB:
        case 0xEF:
                switch (adjacent_sand)
                {
                case 0x04:
                        border_e_horizontal_special_sand_n(pic, mirr);
                        break;
                case 0x80:
                        border_e_horizontal_special_sand_s(pic, mirr);
                        break;
                case 0x00:
                        border_e_horizontal_generic(pic, mirr);
                        break;
                default:
                        border_e_horizontal_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0xF8:
        case 0xF9:
        case 0xFC:
        case 0xFD:
                switch (adjacent_sand)
                {
                case 0x01:
                        border_n_vertical_special_sand_w(pic, mirr);
                        break;
                case 0x04:
                        border_n_vertical_special_sand_e(pic, mirr);
                        break;
                case 0x00:
                        border_n_vertical_generic(pic, mirr);
                        break;
                default:
                        border_n_vertical_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0x1F:
        case 0x3F:
        case 0x9F:
        case 0xBF:
                switch (adjacent_sand)
                {
                case 0x20:
                        border_s_vertical_special_sand_w(pic, mirr);
                        break;
                case 0x80:
                        border_s_vertical_special_sand_e(pic, mirr);
                        break;
                case 0x00:
                        border_s_vertical_generic(pic, mirr);
                        break;
                default:
                        border_s_vertical_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0xD4:
        case 0xF0:
        case 0xF4:
                switch (adjacent_sand)
                {
                case 0x04:
                        border_nw_ext_diagonal_special_sand_short_hori(pic,mirr);
                        break;
                case 0x08:
                case 0x09:
                case 0x28:
                case 0x29:
                        border_nw_ext_diagonal_special_sand_long_vert(pic, mirr);
                        break;
                case 0x02:
                case 0x03:
                case 0x06:
                case 0x07:
                        border_nw_ext_diagonal_special_sand_long_hori(pic,mirr);
                        break;
                case 0x20:
                        border_nw_ext_diagonal_special_sand_short_vert(pic, mirr);
                        break;
                case 0x00:
                        border_nw_ext_diagonal_generic(pic, mirr);
                        break;
                default:
                        border_nw_ext_diagonal_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0xE8:
        case 0xE9:
        case 0x69:
                switch (adjacent_sand)
                {
                case 0x01:
                        border_ne_ext_diagonal_special_sand_short_hori(pic, mirr);
                        break;
                case 0x02:
                case 0x03:
                case 0x06:
                case 0x07:
                        border_ne_ext_diagonal_special_sand_long_hori(pic, mirr);
                        break;
                case 0x10:
                case 0x14:
                case 0x90:
                case 0x94:
                        border_ne_ext_diagonal_special_sand_long_vert(pic,mirr);
                        break;
                case 0x80:
                        border_ne_ext_diagonal_special_sand_short_vert(pic,mirr);
                        break;
                case 0x00:
                        border_ne_ext_diagonal_generic(pic, mirr);
                        break;
                default:
                        border_ne_ext_diagonal_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0x96:
        case 0x97:
        case 0x17:
                switch (adjacent_sand)
                {
                case 0x01:
                        border_sw_ext_diagonal_special_sand_short_vert(pic, mirr);
                        break;
                case 0x08:
                case 0x09:
                case 0x28:
                case 0x29:
                        border_sw_ext_diagonal_special_sand_long_vert(pic, mirr);
                        break;
                case 0xC0:
                case 0xE0:
                case 0x40:
                case 0x60:
                        border_sw_ext_diagonal_special_sand_long_hori(pic,mirr);
                        break;
                case 0x80:
                        border_sw_ext_diagonal_special_sand_short_hori(pic,mirr);
                        break;
                case 0x00:
                        border_sw_ext_diagonal_generic(pic, mirr);
                        break;
                default:
                        border_sw_ext_diagonal_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0x2B:
        case 0x2F:
        case 0x0F:
                switch (adjacent_sand)
                {
                case 0x04:
                        border_se_ext_diagonal_special_sand_short_vert(pic, mirr);
                        break;
                case 0x10:
                case 0x14:
                case 0x90:
                case 0x94:
                        border_se_ext_diagonal_special_sand_long_vert(pic, mirr);
                        break;
                case 0x40:
                case 0x60:
                case 0xC0:
                case 0xE0:
                        border_se_ext_diagonal_special_sand_long_hori(pic,mirr);
                        break;
                case 0x20:
                        border_se_ext_diagonal_special_sand_short_hori(pic,mirr);
                        break;
                case 0x00:
                        border_se_ext_diagonal_generic(pic, mirr);
                        break;
                default:
                        border_se_ext_diagonal_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0xFE:
                (0 == adjacent_sand)? border_nw_int_diagonal_generic(pic, mirr)
                        : border_nw_int_diagonal_special_sand_only(pic, mirr);
                break;
        case 0xFB:
                (0 == adjacent_sand)? border_ne_int_diagonal_generic(pic, mirr)
                        : border_ne_int_diagonal_special_sand_only(pic, mirr);
                break;
        case 0xDF:
                (0 == adjacent_sand)? border_sw_int_diagonal_generic(pic, mirr)
                        : border_sw_int_diagonal_special_sand_only(pic, mirr);
                break;
        case 0x7F:
                (0 == adjacent_sand)? border_se_int_diagonal_generic(pic, mirr)
                        : border_se_int_diagonal_special_sand_only(pic, mirr);
                break;
        case 0x7E:
                switch (adjacent_sand)
                {
                case 0x01:
                        border_nesw_double_diagonal_special_sand_n(pic, mirr);
                        break;
                case 0x80:
                        border_nesw_double_diagonal_special_sand_s(pic, mirr);
                        break;
                case 0x00:
                        border_nwse_double_diagonal_generic(pic, mirr);
                        break;
                default:
                        border_nwse_double_diagonal_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0xDB:
                switch (adjacent_sand)
                {
                case 0x04:
                        border_nwse_double_diagonal_special_sand_n(pic, mirr);
                        break;
                case 0x20:
                        border_nwse_double_diagonal_special_sand_s(pic, mirr);
                        break;
                case 0x00:
                        border_nesw_double_diagonal_generic(pic, mirr);
                        break;
                default:
                        border_nesw_double_diagonal_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0xD0:
                switch (adjacent_sand)
                {
                case 0x04:
                        border_nw_ext_corner_special_sand_short_hori(pic, mirr);
                        break;
                case 0x06:
                case 0x07:
                        border_nw_ext_corner_special_sand_long_hori(pic, mirr);
                        break;
                case 0x20:
                        border_nw_ext_corner_special_sand_short_vert(pic, mirr);
                        break;
                case 0x28:
                case 0x29:
                        border_nw_ext_corner_special_sand_long_vert(pic, mirr);
                        break;
                case 0x00:
                        border_nw_ext_corner_generic(pic, mirr);
                        break;
                default:
                        border_nw_ext_corner_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0x68:
                switch (adjacent_sand)
                {
                case 0x01:
                        border_ne_ext_corner_special_sand_short_hori(pic, mirr);
                        break;
                case 0x03:
                case 0x07:
                        border_ne_ext_corner_special_sand_long_hori(pic, mirr);
                        break;
                case 0x80:
                        border_ne_ext_corner_special_sand_short_vert(pic, mirr);
                        break;
                case 0x90:
                case 0x94:
                        border_ne_ext_corner_special_sand_long_vert(pic, mirr);
                        break;
                case 0x00:
                        border_ne_ext_corner_generic(pic, mirr);
                        break;
                default:
                        border_ne_ext_corner_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0x16:
                switch (adjacent_sand)
                {
                case 0x80:
                        border_sw_ext_corner_special_sand_short_hori(pic, mirr);
                        break;
                case 0xC0:
                case 0xE0:
                        border_sw_ext_corner_special_sand_long_hori(pic, mirr);
                        break;
                case 0x01:
                        border_sw_ext_corner_special_sand_short_vert(pic, mirr);
                        break;
                case 0x09:
                case 0x29:
                        border_sw_ext_corner_special_sand_long_vert(pic, mirr);

                        break;
                case 0x00:
                        border_sw_ext_corner_generic(pic, mirr);
                        break;
                default:
                        border_sw_ext_corner_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0x0B:
                switch (adjacent_sand)
                {
                case 0x20:
                        border_se_ext_corner_special_sand_short_hori(pic, mirr);
                        break;
                case 0x60:
                case 0xE0:
                        border_se_ext_corner_special_sand_long_hori(pic, mirr);
                        break;
                case 0x04:
                        border_se_ext_corner_special_sand_short_vert(pic, mirr);
                        break;
                case 0x14:
                case 0x94:
                        border_se_ext_corner_special_sand_long_vert(pic, mirr);
                        break;
                case 0x00:
                        border_se_ext_corner_generic(pic, mirr);
                        break;
                default:
                        border_se_ext_corner_special_sand_only(pic, mirr);
                        break;
                }
                break;
        case 0xFF:
                borderless_generic(pic);
                break;
        default:
                /* failed to determine border */
                return 1;
        }

        return 0;
}

int dispatch_terrain_dirt(unsigned char adjacent_sand,
                        unsigned char *pic, 
                        unsigned char *mirr)
{
        switch (adjacent_sand)
        {
        case 0x08:
        case 0x09:
        case 0x28:
        case 0x29:
                border_w_horizontal_dirt(pic, mirr);
                break;
        case 0x10:
        case 0x14:
        case 0x90:
        case 0x94:
                border_e_horizontal_dirt(pic, mirr);
                break;
        case 0xC0:
        case 0xE0:
        case 0x40:
        case 0x60:
                border_s_vertical_dirt(pic, mirr);
                break;
        case 0x02:
        case 0x03:
        case 0x06:
        case 0x07:
                border_n_vertical_dirt(pic, mirr);
                break; 
        case 0x2F:
                border_nw_ext_corner_dirt(pic, mirr);
                break;
        case 0xE9:
                border_sw_ext_corner_dirt(pic, mirr);
                break;
        case 0x97:
                border_ne_ext_corner_dirt(pic, mirr);
                break;
        case 0xF4:
                border_se_ext_corner_dirt(pic, mirr);
                break;
        case 0x80:
                border_se_int_diagonal_dirt(pic, mirr);
                break;
        case 0x04:
                border_ne_int_diagonal_dirt(pic, mirr);
                break;
        case 0x20:
                
                border_sw_int_diagonal_dirt(pic, mirr);
                break;
        case 0x01:
                border_nw_int_diagonal_dirt(pic, mirr);
                break;
        case 0x0B:
        case 0x0F:
        case 0x2B:
                border_nw_ext_diagonal_dirt(pic, mirr);
                break;
        case 0x68:
        case 0x69:
        case 0xE8:
                border_sw_ext_diagonal_dirt(pic, mirr);
                break;
        case 0x16:
        case 0x17:
        case 0x96:
                border_ne_ext_diagonal_dirt(pic, mirr);
                break;
        case 0xD0:
        case 0xD4:
        case 0xF0:
                border_se_ext_diagonal_dirt(pic, mirr);
                break;
        case 0x81:
                border_nesw_double_diagonal_dirt(pic, mirr);
                break;
        case 0x24:
                border_nwse_double_diagonal_dirt(pic, mirr);
                break;
        case 0x00:
                borderless_dirt(pic);
                break;
        default:
                /* failed to determine border */
                return 1;
        }

        return 0;
}

int dispatch_terrain_water(unsigned char adjacent_same,
                        unsigned char *pic, 
                        unsigned char *mirr)
{
        switch (adjacent_same)
        {
        case 0xD6:
        case 0xD7:
        case 0xF6:
        case 0xF7:
                border_w_horizontal_water(pic, mirr);
                break;
        case 0x6B:
        case 0x6F:
        case 0xEB:
        case 0xEF:
                border_e_horizontal_water(pic, mirr);
                break;
        case 0xF8:
        case 0xF9:
        case 0xFC:
        case 0xFD:
                border_n_vertical_water(pic, mirr);
                break;
        case 0x1F:
        case 0x3F:
        case 0x9F:
        case 0xBF:
                border_s_vertical_water(pic, mirr);
                break;
        case 0xD4:
        case 0xF0:
        case 0xF4:
                border_nw_ext_diagonal_water(pic, mirr);
                break;
        case 0xE8:
        case 0xE9:
        case 0x69:
                border_ne_ext_diagonal_water(pic, mirr);
                break;
        case 0x96:
        case 0x97:
        case 0x17:
                border_sw_ext_diagonal_water(pic, mirr);
                break;
        case 0x2B:
        case 0x2F:
        case 0x0F:
                border_se_ext_diagonal_water(pic, mirr);
                break;
        case 0xFE:
                border_nw_int_diagonal_water(pic, mirr);
                break;
        case 0xFB:
                border_ne_int_diagonal_water(pic, mirr);
                break;
        case 0xDF:
                border_sw_int_diagonal_water(pic, mirr);
                break;
        case 0x7F:
                border_se_int_diagonal_water(pic, mirr);
                break;
        case 0xD0:
                border_nw_ext_corner_water(pic, mirr);
                break;
        case 0x68:
                border_ne_ext_corner_water(pic, mirr);
                break;
        case 0x16:
                border_sw_ext_corner_water(pic, mirr);
                break;
        case 0x0B:
                border_se_ext_corner_water(pic, mirr);
                break;
        case 0xFF:
                borderless_water(pic);
                break;
        default:
                /* failed to determine border */
                return 1;
        }

        return 0;
}

int dispatch_terrain_rock(unsigned char adjacent_same, unsigned char *pic)
{
        switch (adjacent_same)
        {
        case 0xD6:
        case 0xD7:
        case 0xF6:
        case 0xF7:
                border_w_horizontal_rock(pic);
                break;
        case 0x6B:
        case 0x6F:
        case 0xEB:
        case 0xEF:
                border_e_horizontal_rock(pic);
                break;
        case 0xF8:
        case 0xF9:
        case 0xFC:
        case 0xFD:
                border_n_vertical_rock(pic);
                break;
        case 0x1F:
        case 0x3F:
        case 0x9F:
        case 0xBF:
                border_s_vertical_rock(pic);
                break;
        case 0xD4:
        case 0xF0:
        case 0xF4:
                border_nw_ext_diagonal_rock(pic);
                break;
        case 0xE8:
        case 0xE9:
        case 0x69:
                border_ne_ext_diagonal_rock(pic);
                break;
        case 0x96:
        case 0x97:
        case 0x17:
                border_sw_ext_diagonal_rock(pic);
                break;
        case 0x2B:
        case 0x2F:
        case 0x0F:
                border_se_ext_diagonal_rock(pic);
                break;
        case 0xFE:
                border_nw_int_diagonal_rock(pic);
                break;
        case 0xFB:
                border_ne_int_diagonal_rock(pic);
                break;
        case 0xDF:
                border_sw_int_diagonal_rock(pic);
                break;
        case 0x7F:
                border_se_int_diagonal_rock(pic);
                break;
        case 0xD0:
                border_nw_ext_corner_rock(pic);
                break;
        case 0x68:
                border_ne_ext_corner_rock(pic);
                break;
        case 0x16:
                border_sw_ext_corner_rock(pic);
                break;
        case 0x0B:
                border_se_ext_corner_rock(pic);
                break;
        case 0xFF:
                borderless_rock(pic);
                break;
        default:
                /* failed to determine border */
                return 1;
        }

        return 0;
}

void dispatch_terrain_sand(unsigned char *pic)
{
        borderless_sand(pic);
}

