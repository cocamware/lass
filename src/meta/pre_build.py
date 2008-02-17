import os.path
execfile(os.path.join(os.pardir, os.pardir, 'tools', 'param_expander.py'))

print 'Code generation ...'
N = 15

expandFile('is_member.tmpl.h', 'is_member.h', N)
