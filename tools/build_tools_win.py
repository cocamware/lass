import os
import os.path
import shutil
import re
import sys

PLATFORMS = WIN32, X64 = 'Win32', 'x64'

_VCVAR_PLATFORMS = {
	WIN32: 'x86',
	X64: 'amd64'
	}


_DLL_IGNORE_DIRS = "test", "dll", "frb", "gis"
_DLL_ADDITIONAL_FILES = ["dll\\dll_main.cpp"]



def build_static(msvc_version, solution_file, configurations, do_collect_headers = True):
	return _build_helper(msvc_version, solution_file, configurations, do_collect_headers)



def build_dll(msvc_version, solution_file, project_file, configurations, do_collect_headers = True):
	_update_project_file(project_file)
	return _build_helper(msvc_version, solution_file, configurations, do_collect_headers)



def _build_helper(msvc_version, solution_file, configurations, do_collect_headers):
	logs = {}

	env = _msvc_environment(msvc_version)
	for conf, plat in configurations:
		logs[solution_file, conf, plat] = _build(env, solution_file, conf, plat)	

	if do_collect_headers:
		_collect_headers()

	#print "\n* Overview logs:"
	#for conf, plat in configurations:
	#	print "%s, %s: %s" % (conf, plat, logs[solution_file, conf, plat])
	return logs




class _MsvcEnvironment(object):
	def __init__(self, **kw):
		self.__dict__.update(kw)

def _msvc_environment(msvc_version, platform = WIN32):
	if msvc_version == 7.1:
		assert platform == WIN32, "no x64 platform on MSVC7.1.  At least not configured yet ;)"
		comntools = os.getenv("VS71COMNTOOLS")
		if not comntools:
			return None
		vcvars_path = os.path.normpath(os.path.join(comntools, '..', '..', 'vc7', 'bin', 'vcvars32.bat'))
		vcvars = r'"%s"' % vcvars_path
		
	elif msvc_version == 8.0:
		comntools = os.getenv("VS80COMNTOOLS")
		if not comntools:
			return None
		vcvars_path = os.path.normpath(os.path.join(comntools, '..', '..', 'VC', 'vcvarsall.bat'))
		vcvars = r'"%s" %s' % (vcvars_path, _VCVAR_PLATFORMS[platform])
	else:
		assert False, "MSVC version %s is not configured in the build tools yet" % msvc_version

	devenv_path = os.path.normpath(os.path.join(comntools, '..', 'IDE', 'devenv.exe'))
	devenv = r'"%s"' % devenv_path
		
	return _MsvcEnvironment(version=msvc_version, comntools=comntools, vcvars=vcvars, devenv=devenv)



def _build(msvc_env, solution_file, configuration, platform = WIN32):
	if msvc_env.version >= 8.0:
		conf = '"%s|%s"' % (configuration, platform)
	else:
		conf = configuration
		
	print "\n* Building %s %s ..." % (solution_file, conf)

	log_dir = 'logs'
	log_file = os.path.join(log_dir, 'build.%s.%s.%s.log' % (os.path.basename(solution_file), configuration, platform))
	if not os.path.exists(log_dir):
		os.makedirs(log_dir)
	if os.path.exists(log_file):
		os.remove(log_file)

	if msvc_env is None:
		log = "no compiler found"
	else:
		cmd = r'call %s %s /build %s /out %s' % (msvc_env.devenv, solution_file, conf, log_file)
		print cmd
		os.system(cmd)
		log = _parse_log_file(log_file)
		
	print log
	return log




def _parse_log_file(log_file):
	if os.path.exists(log_file):
		log_text = file(log_file).read()
                match = re.search(r"^=*\s+Build:([\s\w,-]*)", log_text, re.MULTILINE)
		if match:
			return match.group(1).strip()
		return "bad log file"
	return "no log file"




def _collect_headers():
	source_dir = "src"
	header_dir = "lass"
	
	def collector(params, dirname, filenames):
		if dirname.endswith("CVS"):
			return

		def good_filename(x):
			for y in ".h", ".inl":
				if x.endswith(y) and not x.endswith(".tmpl" + y):
					return True
			return False
		headers = filter(good_filename, filenames)
		if not headers:
			return

		source_dir, header_dir = params
		assert dirname.startswith(source_dir)
		destdirname = header_dir + dirname[len(source_dir):]
		if not os.path.exists(destdirname):
			os.makedirs(destdirname)
		for header in headers:
			sourcepath = os.path.join(dirname, header)
			destpath = os.path.join(destdirname, header)
			#print destpath
			shutil.copy2(sourcepath, destpath)

	print "\n* Collecting headers ..."
		
	shutil.rmtree(header_dir, ignore_errors=True)
	os.path.walk(source_dir, collector, (source_dir, header_dir))
		



def _update_project_file(project_file):
	print "* Updating project file %s ..." % project_file
	
	ignore_dirs = list(_DLL_IGNORE_DIRS) + ['.', '..', 'CVS']
	source_dir = "src"
	good_dir = lambda x: not x in ignore_dirs and os.path.isdir(os.path.join(source_dir, x))
	sub_dirs = filter(good_dir, os.listdir(source_dir))
		
	files = _find_sources(source_dir, sub_dirs)
	files += [os.path.join(source_dir, x) for x in _DLL_ADDITIONAL_FILES]
	
	proj = file(project_file)
	project = proj.read()
	proj.close()

	new_project = _insert_files_in_project(project, files)

	if new_project != project:
		print len(project), len(new_project)
		print "SAVE IT!"
		proj = file(project_file, 'w')
		proj.write(new_project)
		proj.close()



def _find_sources(source_dir, sub_dirs):
	def source_finder(arg, dirname, files):
		if dirname.endswith('CVS'):
			return
		sources, ignores = arg
		project_files = filter(lambda x: x.endswith('.vcproj'), files)
		for project_file in project_files:
			files_in_project = _get_files_from_project(os.path.join(dirname, project_file))
			source_files = filter(lambda x: x.endswith('.cpp') and not x in ignores, files_in_project)
			for source_file in source_files:
				sources.append(os.path.normpath(os.path.join(dirname, source_file)))
	sources = []
	for sub_dir in sub_dirs:
		ignores = ("%s.cpp" % sub_dir, "%s_common.cpp" % sub_dir)
		os.path.walk(os.path.join(source_dir, sub_dir), source_finder, (sources, ignores))
	return sources



def _get_files_from_project(project_file):	
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



def _insert_files_in_project(project, files):
	def do_escape(x):
		escaper = re.compile(r'\\', re.DOTALL)
		return escaper.sub(r'\\\\', x)		

	regex = re.compile(r'<Filter(.+?)Name="Source Files"(.*?)>(.+)</Filter>', re.DOTALL)
	files = '\n'.join([r'<File RelativePath="..\..\%s"></File>' % f for f in files])
	repl = r'<Filter\1Name="Source Files"\2>%s</Filter>' % do_escape(files)
	return regex.sub(repl,project)

			
