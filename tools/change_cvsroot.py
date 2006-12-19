# change cvs root
#
# bram.degreve@intec.ugent.be

import sys
import os
import os.path

def visitor(iArg, iDirName, iFiles):
	if iDirName.endswith(os.sep + 'CVS') and 'Root' in iFiles:
		filePath = os.path.join(iDirName, 'Root')
		print filePath
		assert len(list(open(filePath))) == 1
		open(filePath, 'w').write(iArg + '\n')
	
def changeCvsRoot(iProjectPath, iNewCvsRoot):
	os.path.walk(iProjectPath, visitor, iNewCvsRoot)
	
if __name__ == '__main__':
	nargs = len(sys.argv)
	if nargs != 3:
		print 'Usage: %s project_path new_cvsroot' % os.path.basename(sys.argv[0])
	else:
		projectPath = sys.argv[1]
		newCvsRoot = sys.argv[2]
		changeCvsRoot(projectPath, newCvsRoot)
