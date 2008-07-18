# This nice little script is supposed to download, build and install your favourite LASS library from the CVS repository.
#
# To get some help, run this script as following: ./update_lass.py --help
#
# Changelog:
#	9 June 2008, Bram de Greve: 
#		- initial release of install script.
#	12 June 2008, Bram de Greve:
#		- add option to perform a full rebuild instead of an incremental one
#		- collect headers at end of build, as it should have been in the first place =)


import os, os.path, sys

# Default configuration

CVS_ROOT = r":pserver:anonymous@lass.cvs.sourceforge.net:/cvsroot/lass"
CVS_MODULE = "Lass"

LASS_ROOT = os.getenv("LASSROOT") or CVS_MODULE

HAVE_VC71_WIN32 = not os.getenv("VS71COMNTOOLS") is None
HAVE_INTEL700_WIN32 = False
HAVE_VC8_WIN32 = not os.getenv("VS80COMNTOOLS") is None
HAVE_X64 = not os.getenv("ProgramFiles(x86)") is None
HAVE_VC8_X64 = HAVE_VC8_WIN32 and HAVE_X64

if HAVE_X64:
	PROGRAM_FILES = os.getenv("ProgramFiles(x86)")
else:
	PROGRAM_FILES = os.getenv("ProgramFiles")

for dirname in "TortoiseCVS", "CVSNT":
	CVS_BIN = os.path.join(PROGRAM_FILES, dirname, "cvs.exe")
	if os.path.exists(CVS_BIN):
		break

# Command line configuration

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-L", "--lass-root", dest="lass_root", action="store", default=LASS_ROOT, help="The directory where to donwload, build and install LASS.  For example C:\Lass.  By default, it will use the value of the environment variable LASSROOT.  If LASSROOT is not set, it will use the subdirectory Lass of the current directory [default: %s]" % LASS_ROOT)
parser.add_option("-b", "--cvs-bin", dest="cvs_bin", action="store", default=CVS_BIN, help="path of CVS executable. This script tries to automatically find it, but if it fails you can give it a hand. [default: %s]" % CVS_BIN)
parser.add_option("-d", "--cvs-root", dest="cvs_root", action="store", default=CVS_ROOT, help="The location of the CVS repository.  Normally, you should not have to set this one [default: %s]" % CVS_ROOT)
parser.add_option("-m", "--cvs-module", dest="cvs_module", action="store", default=CVS_MODULE, help="CVS module [default: %s]" % CVS_MODULE)
parser.add_option("-r", "--rebuild", dest="rebuild", action="store_true", default=False, help="Rebuild LASS instead of performing an incremental build.")
parser.add_option("-n", "--no-env-var", dest="no_env_var", action="store_true", default=False, help="After building LASS, this script will 'install' it by setting the environment LASSROOT to ... well, the LASS root directory.   Use this flag to skip this step.")
for name in "vc71_win32", "intel700_win32", "vc8_win32", "vc8_x64":
	opt_name = name.replace("_", "-")
	default_value = eval("HAVE_" + name.upper())
	if default_value:
		help_true, help_false = "[DEFAULT] build the %s configuration" % name, "do not build the %s configuration" % name
	else:
		help_true, help_false = "build the %s configuration" % name, "[DEFAULT]  do not build the %s configuration" % name
	parser.add_option("", "--" + opt_name, dest="build_" + name, action="store_true", default=default_value, help=help_true)
	parser.add_option("", "--no-" + opt_name, dest="build_" + name, action="store_false", help=help_false)
options, args = parser.parse_args()




abs_root_dir = os.path.abspath(options.lass_root)
current_dir = os.getcwd()
try:
	# go to parent directory of root dir
	parent_dir = os.path.dirname(abs_root_dir)
	if not os.path.exists(parent_dir):
		os.makedirs(parent_dir)
	os.chdir(parent_dir)

	# update source tree from CVS
	assert os.path.exists(options.cvs_bin), "Cannot find CVS executable '%s'" % options.cvs_bin
	if ' ' in options.cvs_bin and not options.cvs_bin.startswith('"'):
		options.cvs_bin = '"%s"' % options.cvs_bin
	base_dir = os.path.basename(abs_root_dir)
	os.system(r'call %s -z3 -d%s login -p' % (options.cvs_bin, options.cvs_root))
	if not os.path.exists(base_dir):
		cmd = r'call %s -z3 -d%s checkout -P -d %s %s' % (options.cvs_bin, options.cvs_root, base_dir, options.cvs_module)
	else:
		cmd = r'call %s -z3 -d%s update -P -C %s' % (options.cvs_bin, options.cvs_root, base_dir)
	print cmd
	os.system(cmd)

	# import build tools from source tree
	assert os.path.exists(os.path.join(abs_root_dir, 'tools', 'build_tools_win.py'))
	sys.path.append(abs_root_dir)
	from tools.build_tools_win import WIN32, X64, build_static, build_dll, _collect_headers
	os.chdir(abs_root_dir)

	# build VC7.1 configurations
	log_overview = {}
	if options.build_vc71_win32 or options.build_intel700_win32:
		static_configurations = []
		dll_configurations = []
		if options.build_vc71_win32:
			static_configurations += [
				("win32_vc71", WIN32),
				("win32_vc71_d", WIN32)
				]
			dll_configurations += [
				("win32_vc71", WIN32),
				("win32_vc71_d", WIN32)
				]
		if options.build_intel700_win32:
			static_configurations += [
				("win32_intel700", WIN32),
				("win32_intel700_d", WIN32)
				]
			dll_configurations += [
				("win32_intel700", WIN32),
				("win32_intel700_d", WIN32)
				]
		log_overview.update(build_static(7.1, "src\\lass.sln", static_configurations, do_rebuild=options.rebuild, do_collect_headers = False))
		log_overview.update(build_dll(7.1, "src\\dll.sln", "src\\dll\\dll.vcproj", dll_configurations, do_rebuild=options.rebuild, do_collect_headers = False))

	# build VC8 configurations
	if options.build_vc8_win32 or options.build_vc8_x64:
		static_configurations = []
		dll_configurations = []
		if options.build_vc8_win32:
			static_configurations += [
				("vc8", WIN32),
				("vc8_d", WIN32)
				]
			dll_configurations += [
				("vc8", WIN32),
				("vc8_d", WIN32),
				("vc8_sse2", WIN32)
				]

		if options.build_vc8_x64:
			static_configurations += [
				("vc8", X64),
				("vc8_d", X64)
				]
			dll_configurations += [
				("vc8", X64),
				("vc8_d", X64)
				]
		log_overview.update(build_static(8.0, "src\\lass_vc8.sln", static_configurations, do_rebuild=options.rebuild, do_collect_headers = False))
		log_overview.update(build_dll(8.0, "src\\dll_vc8.sln", "src\\dll\\dll_vc8.vcproj", dll_configurations, do_rebuild=options.rebuild, do_collect_headers = False))
		
	# collect headers
	_collect_headers()

	# "install" LASS
	if not options.no_env_var:
		os.putenv("LASSROOT", abs_root_dir)

	# log overview
	print "\n*Overview logs:"
	for sln, conf, plat in sorted(log_overview.keys()):
		print "%s (%s, %s): %s" % (sln, conf, plat, log_overview[sln, conf, plat])	

except:
	os.chdir(current_dir)
	raise
else:
	os.chdir(current_dir)
