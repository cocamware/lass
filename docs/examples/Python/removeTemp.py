# fixVcProj.py
# Author : Tom De Muer (tom_demuer@cocamware.com)
# Copyright : Cocamware 2008-2009.
# All rights reserved.

import re
import os
import string

for root, dirs, files in os.walk("."):
	for name in [file for file in files if os.path.splitext(file)[1] in [".suo",".vcproj",".sln",".user",".obj",".pch",".pyd",".ilk",".lib",".exp",".pdb",".idb",".manifest",".dep",".res"]]:
		os.remove(os.path.join(root,name))


