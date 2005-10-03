#! /usr/bin/python


import os

print 'CLEAN'
os.system('./clean.py')
print 'AUTOCONFISCATE'
os.system('./autoconfiscate_unix.py')
print 'CONFIGURE'
os.system('./configure')
print 'MAKE'
os.system('make')

