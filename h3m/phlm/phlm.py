from ctypes import *
import os # For Debug hack

# z coordinates
Surface = c_int(0)
Underground = c_int(1)

# Terrains
Dirt = c_int(0)
Sand = c_int(1)
Grass = c_int(2)
Snow = c_int(3)
Swamp = c_int(4)
Rough = c_int(5)
Subterranean = c_int(6)
Lava = c_int(7)
Water = c_int(8)
Rock = c_int(9)

# Rivers
Clear = c_int(1)
Icy = c_int(2)
Muddy = c_int(3)
Lava = c_int(4)

# Roads
# Dirt is already defined in Terrains... So we simply use that define
# Which means all road defines are -1 of h3m values
Gravel = c_int(1)
Cobblestone = c_int(2)

# Players
Red = c_int(0)
Blue = c_int(1)
Tan = c_int(2)
Green = c_int(3)
Orange = c_int(4)
Purple = c_int(5)
Teal = c_int(6)
Pink = c_int(7)

# Monster disposition
AlwaysJoin = c_int(0)
LikelyJoin = c_int(1)
MayJoin = c_int(2)
UnlikelyJoin = c_int(3)
NeverJoin = c_int(4)

class HeroesMap(object):	
	""" 
		Highlevel class for  Heroes 3 maps
	"""
	dll = None

	def __init__(self, size, underground):
		#os.chdir("..\h3mtools\Debug") # Debug hack
		os.chdir("..\h3mtools\Release") # Debug hack
		self.dll = CDLL("hlm.dll")
		#self.dll = CDLL("hlm.so")

		# TODO put all these C function declarations in its their own file,
		# preferably auto-generated...

		self.dll.hlm_init_defaults.argtypes = [c_int, c_int]
		self.dll.hlm_init_defaults.res_type = None

		self.dll.hlm_xyz_add_object.argtypes = [c_int, c_int, c_int, c_char_p]
		self.dll.hlm_xyz_add_object.res_type = c_int

		self.dll.hlm_last_set_owner.argtypes = [c_int]
		self.dll.hlm_last_set_owner.res_type = c_int

		self.dll.hlm_last_set_quantity.argtypes = [c_int]
		self.dll.hlm_last_set_quantity.res_type = c_int

		self.dll.hlm_last_set_disposition.argtypes = [c_int]
		self.dll.hlm_last_set_disposition.res_type = c_int

		self.dll.hlm_last_set_creatures.argtypes = [c_int, c_char_p, c_int]
		self.dll.hlm_last_set_creatures.res_type = c_int
		
		self.dll.hlm_last_set_random_creatures.argtypes = []
		self.dll.hlm_last_set_random_creatures.res_type = c_int

		self.dll.hlm_save_h3m.argtypes = [c_char_p]
		self.dll.hlm_save_h3m.res_type = c_int

		self.dll.hlm_fill_terrain.argtypes = [c_int, c_int]
		self.dll.hlm_fill_terrain.restype = None

		self.dll.hlm_xyz_gen_island_terrain.argtypes = [c_int, c_int, c_int, c_int, c_int]
		self.dll.hlm_xyz_gen_island_terrain.res_type = c_int

		self.dll.hlm_xyz_set_terrain.argtypes = [c_int, c_int, c_int, c_int]
		self.dll.hlm_xyz_set_terrain.res_type = c_int

		self.dll.hlm_xyz_set_road.argtypes = [c_int, c_int, c_int, c_int]
		self.dll.hlm_xyz_set_road.res_type = c_int

		self.dll.hlm_xyz_set_river.argtypes = [c_int, c_int, c_int, c_int]
		self.dll.hlm_xyz_set_river.res_type = c_int

		self.dll.hlm_xyz2_set_terrain.argtypes = [c_int, c_int, c_int, c_int, c_int, c_int]
		self.dll.hlm_xyz2_set_terrain.res_type = c_int

		self.dll.hlm_xyz2_set_road.argtypes = [c_int, c_int, c_int, c_int, c_int, c_int]
		self.dll.hlm_xyz2_set_road.res_type = c_int

		self.dll.hlm_xyz2_set_river.argtypes = [c_int, c_int, c_int, c_int, c_int, c_int]
		self.dll.hlm_xyz2_set_river.res_type = c_int

		self.dll.hlm_init_defaults(size, underground)
	def __exit__(self):
		dll.hlm_exit()
	"""
		Basic properties interface
	"""
	def size(self):
		""" Returns map size """
		return 0
	def player_count(self):
		""" Returns player count"""
		return 0
	def fill_terrain(self, z, terrain):
		return self.dll.hlm_fill_terrain(z, terrain)
	"""
		xyz interface - Operates on coordinates.
	"""
	def xyz_add_town(self, x, y, z, owner = "Neutral", alignment = "Random", has_fort = True, visiting_hero = None):
		"""
			Adds Town. 

			owner argument can be strings "Neutral", "Red", "Blue" etc.
			or number -1 for Neutral, 0 for Red, 1 for Blue etc.

			alignment argument can be "Random", "Castle", "Rampart" etc
		"""
		return 0 #dll.hlm.xyz_add_town(x, y, z, owner, alignment, has_fort, visiting_hero)
	def xyz_add_tree(self, x, y, z, n = -1):
		""" Adds Tree """
		return 0 #self.hlm.xyz_add_tree(x, y, z, n)
	def xyz_add_object(self, x, y, z, name):
		"""
			Adds object of the specified name. 
		"""
		return self.dll.hlm_xyz_add_object(x, y, z, name.encode("iso-8859-1"))
	def xyz_clear_objects(self, xyz):
		"""
			Clears the specified coordinates of any objects
			that may (in part or fully) reside there.
		"""
		return 0 #self.hlm.xyz_clear_objects(xyz)
	def xyz_get_objects(self, xyz):
		return 0 #self.hlm.xyz_get_objects(xyz)
	def xyz_delete_object(self, x, y, z, name):
		pass
	def xyz_set_terrain(self, x, y, z, terrain):
		"""
			Sets the terrain of the specified coordinate. 
			Accepts: Dirt, Sand, Grass, Snow, Swamp, Rough, Subterranean, Lava,
			         Water, Rock
		"""
		return self.dll.hlm_xyz_set_terrain(x, y, z, terrain)
	def xyz_set_road(self, x, y, z, type = Dirt):
		"""
			Sets the road of the specified coordinate. 
			Accepts: None, Dirt, Gravel, Cobblestone
		"""
		if type == None:
			type = c_int(-1)
		return self.dll.hlm_xyz_set_road(x, y, z, type.value+1) # + 1 because of Dirt
	def xyz_set_river(self, x, y, z, type = Dirt):
		"""
			Sets the road of the specified coordinate. 
			Accepts: None, Clear, Icy, Muddy, Lava
		"""
		if type == None:
			type = c_int(0)
		return self.dll.hlm_xyz_set_river(x, y, z, type)
	def xyz_gen_island_terrain(self, x, y, z, terrain, perimeter):
		"""
			Generates a randomly shaped island of the specified terrain
			within the provided perimeter. If called multiple times the islands
			are kept at a distance from each other.
			(TODO rule api to set this distance).
		"""
		return self.dll.hlm_xyz_gen_island_terrain(x, y, z, terrain, perimeter)
	"""
		xyz2 interface - Operates on two sets of coordinates, on each tile
		                 from the first to the second coordinate.
	"""
	def xyz2_set_terrain(self, x1, y1, x2, y2, z, terrain):
		""" 
			Creates a terrain horizontally, vertically or diagonally
			from one coordinate to another. 
			
			NOTE that there needs to be several adjacent tiles for terrain or 
			it cannot exist, see what happens when you try to place e.g a row 
			of terrain in the map editor and you will see that terrain need
			adjacent tiles of the same terrain (or they need to be at map edge).
		"""
		return self.dll.hlm_xyz2_set_terrain(x1, y1, x2, y2, z, terrain)
	def xyz2_set_river(self, x1, y1, x2, y2, z, river):
		""" 
			Creates a river horizontally, vertically or diagonally
			from one coordinate to another.
		"""
		return self.dll.hlm_xyz2_set_river(x1, y1, x2, y2, z, river)
	def xyz2_set_road(self, x1, y1, x2, y2, z, road):
		""" 
			Creates a road horizontally, vertically or diagonally
			from one coordinate to another.
		"""
		return self.dll.hlm_xyz2_set_road(x1, y1, x2, y2, z, road)
	def xyz2_add_monolith_couple(self, xyz1, xyz2, type = "Small One Way", n = -1):
		"""
			Adds a One Way Monolith pair, with the entrance placed
			at the first provided coordinate and the exit at the second.
		"""
		return 0 #self.hlm.xyz2_add_monolith_couple(xyz1, xyz2, type, n)
	"""
		last interface - Operates on the last added object.
	"""
	def last_set_owner(self, owner):
		return self.dll.hlm_last_set_owner(owner)
	def last_set_quantity(self, quantity):
		return self.dll.hlm_last_set_owner(quantity)
	def last_set_disposition(self, disposition):
		return self.dll.hlm_last_set_disposition(disposition)
	def last_set_creatures(self, slot, name, quantity):
		return self.dll.hlm_last_set_creatures(slot, name.encode("iso-8859-1"), quantity)
	def last_set_random_creatures(self):
		return self.dll.hlm_last_set_random_creatures()
	"""
		Other (TODO: sort etc)
	"""
	def all_delete_object(self, name):
		pass

	def saveH3M(self, filename):
		return self.dll.hlm_save_h3m(filename.encode("iso-8859-1"))

class HmapError(Exception):
	def __init__(self, value = 0):
		self.value = value
	def __str__(self):
		return repr(self.value)
