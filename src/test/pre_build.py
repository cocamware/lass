execfile(r'../../tools/param_expander.py')

print 'Code generation ...'

expandFile('test_util_callback.tmpl.inl', 'test_util_callback.inl', 3)

