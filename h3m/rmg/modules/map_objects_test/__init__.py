# Generic __init__.py for rmg modules, simply copy into new modules.

# src: http://stackoverflow.com/questions/279237/import-a-module-from-a-relative-path/6098238#6098238
import os, sys, inspect
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
	sys.path.insert(0, cmd_folder)

from gen import *