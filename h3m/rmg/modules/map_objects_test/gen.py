from phlm import *

def gen(settings):
	map = HeroesMap()

	map.xyz_add_object(6, 5, 0, "Market of Time")
	map.xyz_add_object(10, 5, 0, "Abandoned Mine")

	map.xyz_add_object(5, 0, 0, "Wolf Pen")

	return map
