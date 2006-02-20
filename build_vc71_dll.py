import os
import os.path
import re

projects = ['io', 'num', 'prim', 'spat', 'stde', 'util']
project_file = 'src/dll/dll.vcproj'
configurations = ['win32_vc71', 'win32_vc71_d']
ignores = ['%s.cpp' % p for p in projects] + ['%s_common.cpp' % p for p in projects]
			
def get_files_from_project(project_file):	
	import xml.sax	
	class vcproj_handler(xml.sax.handler.ContentHandler):
		def __init__(self):
			self.files = []
			self.names = []	
		def startElement(self, name, attrs):
			self.names.append(str(name))
			self.current_characters = ''
			if name == 'File' and self.names[:2] == ['VisualStudioProject', 'Files']:
				self.files.append(str(attrs['RelativePath']))		
		def characters(self, ch): 
			self.current_characters += ch		
		def endElement(self, name):
			assert(name == self.names[-1])
			self.names = self.names[:-1]
	handler = vcproj_handler()
	xml.sax.parse(project_file, handler)
	return handler.files

def find_sources(subdirs, ignores = []):
	def source_finder(arg, dirname, files):
		project_files = filter(lambda x: x.endswith('.vcproj'), files)
		for project_file in project_files:
			files_in_project = get_files_from_project(os.path.join(dirname, project_file))
			source_files = filter(lambda x: x.endswith('.cpp') and not x in ignores, files_in_project)
			for source_file in source_files:
				arg.append(os.path.join(dirname, source_file))					
	sources = []
	for subdir in subdirs:
		os.path.walk(subdir, source_finder, sources)
	return sources



def insert_files_in_project(project, files):
	def do_escape(x):
		escaper = re.compile(r'\\', re.DOTALL)
		return escaper.sub(r'\\\\', x)		

	regex = re.compile(r'<File .+</File>', re.DOTALL)
	repl = '\n'.join([r'<File RelativePath="..\..\%s"></File>' % f for f in files])
	return regex.sub(do_escape(repl),project)
	
	

# update project file

subdirs = [os.path.join('src', p) for p in projects]
files = find_sources(subdirs, ignores)

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
