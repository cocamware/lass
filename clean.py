# deletes all builded files of all projects

import os

print "cleaning up builds ..."

projects = ['frb', 'gis', 'io', 'num', 'prim', 'spat', 'stde', 'test', 'util'];
configurations = ['win32_vc7', 'win32_vc7_d', 'win32_vc71', 'win32_vc71_d', 'win32_intel700', 'win32_intel700_d']

os.chdir('lib')
os.system('del *.lib /F /Q')
os.system('del *.pdb /F /Q')
os.chdir('..')

os.chdir('src')
for p in projects:
    for c in configurations:
        os.system('rmdir ' + p + '\\' + c + ' /S /Q')
os.chdir('..')


print "cleaning up documentation ..."

os.chdir('docs\\doxygen')
os.system('rmdir html /S /Q')
os.chdir('..\\..')
