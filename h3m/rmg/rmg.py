import sys
sys.path.append("../phlm/")
from phlm import *

from settings import *
from dispatch_gen import *

def print_usage():
	print("Usage: " + str(sys.argv[0]) + " [size] [Red town] [Red hero] [Blue town] [Blue hero], ...")
	print("Default: XL map with random Red, Blue, Green and Tan")
	print("Examples: ")
	print("\t"+ str(sys.argv[0]) + " L Necropolis Tamika")
	print("\t"+ str(sys.argv[0]) + " XL Necropolis Tamika Random Random Random Random Random Random")
	print("\t"+ str(sys.argv[0]) + " XLU Necropolis Tamika Castle Orrin Random Random Random Random Random Random Random Random")

if __name__ == "__main__":
	if len(sys.argv) > 1:
		if "-h" == sys.argv[1] or "--help" == sys.argv[1]:
			print_usage()
			exit(1)
		else:
			filename = sys.argv[1]
	else:
		filename = "random_map.h3m"

	# Create settings class from cmd args
	settings = Settings(sys.argv)

	# Have module generate map
	map = dispatch_gen(settings)

	map.saveH3M(filename)
	print("Done")