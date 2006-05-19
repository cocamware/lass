# rebuilds all configurations of all projects

import os

os.system('python clean.py')
os.system('python build_vc71.py')
os.system('python build_intel700.py')
os.system('python build_vc71_dll.py')
os.system('python build_intel700_dll.py')
print 'done'