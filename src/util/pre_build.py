import os.path
execfile(os.path.join(os.pardir, os.pardir, 'tools', 'param_expander.py'))

print 'Code generation ...'

expandFile('object_factory.tmpl.h', 'object_factory.h', 10)
expandFile('clone_factory.tmpl.h', 'clone_factory.h', 10)

expandFile('callback.tmpl.h', 'callback.h', 10)
expandFile('callback_x.tmpl.h', 'callback_$x.h', 10)
expandFile('callback_r_x.tmpl.h', 'callback_r_$x.h', 10)
expandFile('callback_python.tmpl.h', 'callback_python.h', 10)
expandFile('callback_python.tmpl.inl', 'callback_python.inl', 10)
expandFile('impl/dispatcher_x.tmpl.h', 'impl/dispatcher_$x.h', 10)
expandFile('impl/dispatcher_r_x.tmpl.h', 'impl/dispatcher_r_$x.h', 10)
expandFile('impl/dispatcher_python.tmpl.h', 'impl/dispatcher_python.h', 10)

expandFile('pyobject_macros.tmpl.h', 'pyobject_macros.h', 10)
expandFile('pyobject_call.tmpl.inl', 'pyobject_call.inl', 10)
expandFile('py_tuple.tmpl.h', 'py_tuple.h', 20)

expandFile('thread_fun.tmpl.h', 'thread_fun.h', 10)
expandFile('thread_fun.tmpl.inl', 'thread_fun.inl', 10)
