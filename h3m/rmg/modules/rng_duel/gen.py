from phlm import *

def gen(settings):
	map = HeroesMap()

	map.xyz_add_object(5, 5, 0, "Random Hero")
	map.last_set_owner(Red)
	map.last_set_random_creatures()

	map.xyz_add_object(6, 5, 0, "Random Hero")
	map.last_set_owner(Blue)
	map.last_set_random_creatures()

	return map
