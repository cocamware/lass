#! /usr/bin/env python

from tools.param_expander import expandFile
import os.path

def pre_build():
	print 'Code generation ...'
	N = 15
	srcDir = 'src'
	
	utilDir = os.path.join(srcDir, 'util')
	expandFile(utilDir, 'object_factory.tmpl.h', 'object_factory.h', N)
	expandFile(utilDir, 'clone_factory.tmpl.h', 'clone_factory.h', N)
	expandFile(utilDir, 'callback.tmpl.h', 'callback.h', N)
	expandFile(utilDir, 'callback_x.tmpl.h', 'callback_$x.h', N)
	expandFile(utilDir, 'callback_r_x.tmpl.h', 'callback_r_$x.h', N)
	expandFile(utilDir, 'callback_python.tmpl.h', 'callback_python.h', N)
	expandFile(utilDir, 'pyobject_macros.tmpl.h', 'pyobject_macros.h', N)
	expandFile(utilDir, 'pyobject_call.tmpl.inl', 'pyobject_call.inl', N)
	expandFile(utilDir, 'py_tuple.tmpl.h', 'py_tuple.h', 20)
	expandFile(utilDir, 'thread_fun.tmpl.h', 'thread_fun.h', N)
	expandFile(utilDir, 'thread_fun.tmpl.inl', 'thread_fun.inl', N)
	expandFile(utilDir, 'bind.tmpl.h', 'bind.h', N)
	
	utilImplDir = os.path.join(utilDir, 'impl')
	expandFile(utilImplDir, 'dispatcher_x.tmpl.h', 'dispatcher_$x.h', N)
	expandFile(utilImplDir, 'dispatcher_r_x.tmpl.h', 'dispatcher_r_$x.h', N)
	
	metaDir = os.path.join(srcDir, 'meta')
	expandFile(metaDir, 'is_member.tmpl.h', 'is_member.h', N)
	
	testDir = os.path.join(srcDir, 'test')
	expandFile(testDir, 'test_util_callback.tmpl.inl', 'test_util_callback.inl', 3)


if __name__ == "__main__":
	pre_build()