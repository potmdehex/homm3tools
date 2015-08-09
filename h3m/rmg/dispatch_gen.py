import sys
from random import *

def dispatch_gen(settings, rmgseed = 0):
	"""
		Short function that prepares map generation
		and then dispatches to generation modules
	"""
	
	# Seed with a specific seed if one was provided
	if rmgseed != 0:
		print("seed:" + str(rmgseed))
		seed(rmgseed)

	module_name = "modules.%s.__init__" % settings.module_name
	rmg_module = __import__(module_name, fromlist=[module_name])
	
	max_attempts = 5
	for i in range(max_attempts):
		hmap = None
		print("[i]At attempt %d of %d for for module %s to generate given these settings" % (i+1, max_attempts, settings.module_name))
		hmap = rmg_module.gen(settings)
		if hmap != None:
			print("[+]Successful at attempt %d/%d for module %s to generate given these settings"  % (i+1, max_attempts, settings.module_name))
			break
		else:
			print("[!]Failed at attempt %d/%d for module %s to generate given these settings" % (i+1, max_attempts, settings.module_name))
		
	return hmap
