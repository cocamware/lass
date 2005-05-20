import os
import os.path
import re

projects = ['frb', 'gis', 'io', 'num', 'prim', 'spat', 'stde', 'util']
project_file = 'src/dll/dll.vcproj'
source_dir = r'src'
configurations = ['win32_vc71', 'win32_vc71_d']


def gather_source_files():
	def walker(arg, dirname, files):
		dirname = os.path.join(os.path.pardir, dirname[4:])
		if not dirname.endswith('test'):
			arg += [os.path.join(dirname, f) for f in files if f.endswith('.cpp')]		
	def remover(files, suffix):
		return [f for f in files if not f.endswith(suffix)]
	
	result = []
	os.path.walk(source_dir, walker, result)
	for p in projects:
		result = remover(result, p + '.cpp')
		result = remover(result, p + '_common.cpp')
	result = remover(result, 'thread_unix.cpp')
	result = remover(result, 'thread_win32.cpp')
	result = remover(result, 'matlab_o_stream.cpp')	
	result = remover(result, 'matlab_o_stream.cpp')	
	return result

def insert_files_in_project(project, files):
	def do_escape(x):
		escaper = re.compile(r'\\', re.DOTALL)
		return escaper.sub(r'\\\\', x)		
	regex = re.compile(r'<File .+</File>', re.DOTALL)
	repl = '\n'.join(['<File RelativePath="%s"></File>' % f for f in files])
	return regex.sub(do_escape(repl),project)
	
	

# update project file

files = gather_source_files()

proj = file(project_file)
project = proj.read()
proj.close()

project = insert_files_in_project(project, files)

proj = file(project_file, 'w')
proj.write(project)
proj.close()

# build project file

print os.getenv('vs71comntools')
vs71_dir = '\\'.join(os.getenv('vs71comntools').split('\\')[:-3])

os.system(r'"%s\vc7\bin\vcvars32.bat"' % vs71_dir)
os.chdir('src')
for c in configurations:
	print '*', c
	os.system(r'"%s\Common7\IDE\devenv.exe" /rebuild %s /out %s.dll.log dll.sln' % (vs71_dir, c, c))

os.chdir('..')

os.system('mkdir logs')
os.system('copy src\\*.dll.log logs')
os.system('del src\\*.dll.log /F /Q')