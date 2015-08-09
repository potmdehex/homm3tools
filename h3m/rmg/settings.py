SIZE_MEDIUM = 36
SIZE_LARGE = 74
MAX_PLAYER_COUNT = 8

class Settings:
	# Defaults
	size = SIZE_LARGE
	has_two_levels = 0
	player_count = 4
	player_town = ["Random" for x in range(MAX_PLAYER_COUNT)]
	player_hero = ["Random" for x in range(MAX_PLAYER_COUNT)]
	# TODO: Actually try to retrieve rng_module from command line
	module_name = "test"
	
	def __init__(self, cmd_args):
		# First arg is script name - not needed here
		# Second arg is output file - not needed here
		# Third arg is map size
		if len(cmd_args) >= 2:
			self.size = int(cmd_args[2])
		# Rest of args are <<town> <hero>> for all players
		if len(cmd_args) >= 3:
			self.player_count = 0
			cur_player = 0
			next_is_town = True
			for i in range(3, len(cmd_args)):
				print ("@" + cmd_args[i])
				if next_is_town == True:
					self.player_town[cur_player] = cmd_args[i]
					self.player_count += 1
					next_is_town = False
				else:
					self.player_hero[cur_player] = cmd_args[i]
					cur_player += 1
					next_is_town = True
		return
