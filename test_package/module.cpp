#include <lass/python/python_api.h>

double multiply(double a, double b)
{
	return a * b;
}

PY_DECLARE_MODULE(test_module)
PY_MODULE_FUNCTION(test_module, multiply)
PY_MODULE_ENTRYPOINT(test_module)
