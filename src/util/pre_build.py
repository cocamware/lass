import os.path
execfile(os.path.join(os.pardir, os.pardir, 'tools', 'param_expander.py'))

print 'Code generation ...'
N = 15

expandFile('object_factory.tmpl.h', 'object_factory.h', N)
expandFile('clone_factory.tmpl.h', 'clone_factory.h', N)

expandFile('callback.tmpl.h', 'callback.h', N)
expandFile('callback_x.tmpl.h', 'callback_$x.h', N)
expandFile('callback_r_x.tmpl.h', 'callback_r_$x.h', N)
expandFile('callback_python.tmpl.h', 'callback_python.h', N)
expandFile('callback_python.tmpl.inl', 'callback_python.inl', N)
expandFile('impl/dispatcher_x.tmpl.h', 'impl/dispatcher_$x.h', N)
expandFile('impl/dispatcher_r_x.tmpl.h', 'impl/dispatcher_r_$x.h', N)

expandFile('pyobject_macros.tmpl.h', 'pyobject_macros.h', N)
expandFile('pyobject_call.tmpl.inl', 'pyobject_call.inl', N)
expandFile('py_tuple.tmpl.h', 'py_tuple.h', 20)

expandFile('thread_fun.tmpl.h', 'thread_fun.h', N)
expandFile('thread_fun.tmpl.inl', 'thread_fun.inl', N)

expandFile('bind.tmpl.h', 'bind.h', N)
