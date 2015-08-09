#ifndef __H3M_TERRAIN_H_DEF__
#define __H3M_TERRAIN_H_DEF__

enum
{
	T_DIRT,
	T_SAND,
	T_GRASS,
	T_SNOW,
	T_SWAMP,
	T_ROUGH,
	T_SUBTERRANEAN,
	T_LAVA,
	T_WATER,
	T_ROCK
};

/* For mirroring of terrain in H3M_TERRAIN */
#define BIT_HORIZONTAL  0x01
#define BIT_VERTICAL    0x02
#define BIT_HORI        0x01
#define BIT_VERT        0x02

struct H3M_TILE
{
	unsigned char terrain_type;
	unsigned char terrain_sprite;
	unsigned char river_type;
	unsigned char river_sprite;
	unsigned char road_type;
	unsigned char road_sprite;
	unsigned char mirroring;
};

#endif

