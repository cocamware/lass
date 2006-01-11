#! /usr/bin/python


# deletes all builded files of all projects

import os

generated_files = ['util/object_factory.h', 'util/clone_factory.h', 'util/pyobject_macros.h', 'util/pyobject_class.inl', 
	'util/callback_python.h']
generated_files += ['util/callback_%d.h' % i for i in range(1, 11)] + ['util/callback_r_%d.h' % i for i in range(1, 11)]

def remove_tree(dirpath):
	if dirpath[-1] == os.sep: dirpath = dirpath[:-1]
	try:
        	items = os.listdir(dirpath)
	except:
		return
        for f in items:
                if f == os.curdir or f == os.pardir: continue
                path = dirpath + os.sep + f
                if os.path.isdir(path):
                        remove_tree(path)
                else:
                        os.remove(path)
        os.rmdir(dirpath)



def clean_win32():
	print "cleaning up win32 build ..."

	projects = ['frb', 'gis', 'io', 'num', 'prim', 'spat', 'stde', 'test', 'util'];
	configurations = ['win32_vc7', 'win32_vc7_d', 'win32_vc71', 'win32_vc71_d', 'win32_intel700', 'win32_intel700_d']

	os.chdir('bin')
	os.system('del *.exe /F /Q')
	os.system('del *.dll /F /Q')
	os.system('del *.pdb /F /Q')
	os.chdir('..')

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

		
def clean_unix():
	print "cleaning up unix build ..."

	try:
		os.system('make distclean')
	except:
		pass

	print "removing files ..."

	files_to_be_removed = ['aclocal.m4', 'config.guess', 'config.h', 'config.h.in', 'config.log', 
		'config.status', 'config.sub', 'configure', 'configure.ac', 'depcomp', 'install-sh',
		'libtool', 'ltmain.sh', 'missing', 'mkinstalldirs', 'stamp-h1', 'tmp', 
		'lass-?.?.?.pc.in', 'lass-?.?.?.tar.gz']
	files_to_be_removed += [os.path.join('src', x) for x in generated_files]
	for x in files_to_be_removed:
		try:
			os.remove(x)
		except:
			pass
			
	dirs_to_be_removed = ['autom4te.cache']
	for x in dirs_to_be_removed:
		remove_tree(x)
		
	files_to_be_searched_and_destroyed = ['Makefile', 'Makefile.am', 'Makefile.in']
	def file_destroyer(targets, dirname, files):
		for f in files:
			if f in targets:
				os.remove(os.path.join(dirname, f))
	os.path.walk('.', file_destroyer, files_to_be_searched_and_destroyed)
	
	extensions_to_be_searched_and_destroyed = ['am', 'in', 'o', 'lo', 'Po', 'Plo', 'a', 'la', 'Pa', 'Pla', 'gz']
	def extension_destroyer(targets, dirname, files):
		for f in files:
			ext = os.path.splitext(f)[1]
			if ext and ext[1:] in targets:
				os.remove(os.path.join(dirname, f))
	os.path.walk('.', extension_destroyer, extensions_to_be_searched_and_destroyed)

#################################################

if __name__ == '__main__':
	# os table
	os_to_clean = {
		'win32': clean_win32,
                'nt': clean_win32,
		'posix': clean_unix
		}
		
	# get os name
	import sys
	argc = len(sys.argv)
	if argc > 1:
		if argc == 2:
			os_name = sys.argv[1]
		else:
			os_name = None
	else:
		os_name = os.name
	# select correct clean
	if os_name and os_name in os_to_clean:
		cleaner = os_to_clean[os_name]
		cleaner()
	else:
		print "Usage: %s [{%s}]" % (os.path.basename(sys.argv[0]), ','.join(os_to_clean.keys()))
		print " OS name '%s' is not recognized, please provide correct OS name." % os_name
