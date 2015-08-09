from phlm import *

def level_columns(map):
	for i in range(7):
		map.xyz_add_object(7, 4+i, 0, "Random Monster" + str(i+1)) # level specified by string append
		map.last_set_disposition(AlwaysJoin)

	for i in range(7):
		map.xyz_add_object(13, 4+i, 0, "Random Monster" + str(i+1)) # level specified by string append
		map.last_set_disposition(AlwaysJoin)

def gen(settings):
	map = HeroesMap()

	map.fill_terrain(Surface, Snow)

	map.xyz_add_object(11, 6, 0, "Random Hero")
	map.last_set_owner(Red)

	map.xyz_add_object(11, 7, 0, "Random Hero")
	map.last_set_owner(Blue)

	level_columns(map)

	return map
