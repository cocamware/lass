#include <lass/python/python_api.h>

double multiply(double a, double b)
{
	return a * b;
}

PY_DECLARE_MODULE(test_module)
PY_MODULE_FUNCTION(test_module, multiply)

#ifdef Py_LIMITED_API
PY_MODULE_INTEGER_CONSTANT(test_module, "Py_LIMITED_API", Py_LIMITED_API+0)
#else
PY_MODULE_INTEGER_CONSTANT(test_module, "Py_LIMITED_API", 0)
#endif

PY_MODULE_ENTRYPOINT(test_module)
