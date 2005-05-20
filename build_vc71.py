# rebuilds vc7 configurations of all projects

import os

msvcPath = 'C:\\Program Files\\Microsoft Visual Studio .NET 2003'
configurations = ['win32_vc71', 'win32_vc71_d']

print "rebuild lass vc7 ..." 

os.chdir('src')
os.system('"' + msvcPath + '\\vc7\\bin\\vcvars32.bat"')
for c in configurations:
    print '*', c
    os.system('"' + msvcPath + '\\Common7\\IDE\\devenv.exe" /rebuild ' + c + ' /out ' + c + '.log lass.sln')
os.chdir('..')

os.system('mkdir logs')
os.system('copy src\\*.log logs /Y')
os.system('del src\\*.log /F /Q')
