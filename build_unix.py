#! /usr/bin/python


import os
import sys

print '\n* CLEAN'
os.system(sys.executable + ' clean.py')
print '\n* AUTOCONFISCATE'
os.system(sys.executable + ' autoconfiscate_unix.py')
print '\n* CONFIGURE'
os.system('./configure')
print '\n* MAKE'
os.system('make')

