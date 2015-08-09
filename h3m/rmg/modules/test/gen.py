from phlm import *

def gen(settings):
	map = HeroesMap()

	map.xyz_set_terrain(0,0,0, Snow)
	map.xyz_set_road(5, 5, 0, Dirt)
	map.xyz_set_river(6, 5, 0, Clear)

	map.xyz2_set_road(5, 7, 10, 7, 0, Gravel)
	map.xyz2_set_river(5, 8, 10, 8, 0, Icy)

	map.xyz2_set_terrain(4, 7, 11, 7, 0, Snow)
	map.xyz2_set_terrain(4, 8, 11, 8, 0, Snow)
	map.xyz2_set_terrain(4, 9, 11, 9, 0, Snow)

	return map
