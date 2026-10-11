#include <lass/python/python_api.h>
#include <iostream>

int main()
{
	using namespace lass;

#ifdef LASS_PY_LIMITED_API
	constexpr long lass_py_limited_api = LASS_PY_LIMITED_API+0;
#else
	constexpr long lass_py_limited_api = 0;
#endif

#ifdef Py_LIMITED_API
	constexpr long exe_py_limited_api = Py_LIMITED_API+0;
#else
	constexpr long exe_py_limited_api = 0;
#endif
	if (exe_py_limited_api != lass_py_limited_api)
	{
		std::cerr << "test_package Py_LIMITED_API=" << exe_py_limited_api << " != LASS_PY_LIMITED_API=" << lass_py_limited_api  << std::endl;
		return 1;
	}

	Py_Initialize();

	python::TPyObjPtr mod(PyImport_ImportModule("test_module"));
	if (!mod)
	{
		PyErr_Print();
		return 1;
	}

	python::TPyObjPtr mod_py_limited_api_obj(PyObject_GetAttrString(mod.get(), "Py_LIMITED_API"));
	long mod_py_limited_api;
	if (!mod_py_limited_api_obj || python::pyGetSimpleObject(mod_py_limited_api_obj.get(), mod_py_limited_api) != 0)
	{
		PyErr_Print();
		return 1;
	}
	if (mod_py_limited_api != lass_py_limited_api)
	{
		std::cerr << "test_module Py_LIMITED_API=" << mod_py_limited_api << " != LASS_PY_LIMITED_API=" << lass_py_limited_api  << std::endl;
		return 1;
	}

	python::TPyObjPtr multiply(PyObject_GetAttrString(mod.get(), "multiply"));
	if (!multiply)
	{
		PyErr_Print();
		return 1;
	}

	auto args = python::makeTuple(6, 0.5);
	python::TPyObjPtr obj(PyObject_CallObject(multiply.get(), args.get()));
	if (!obj)
	{
		PyErr_Print();
		return 1;
	}
	double value;
	if (python::pyGetSimpleObject(obj.get(), value) != 0)
	{
		PyErr_Print();
		return 1;
	}
	if (value == 3.0) // 6 * 0.5 is exact
	{
		std::cerr << "OK\n";
		return 0;
	}
	else
	{
		std::cerr << "Uh-oh, expected 3.0 as result, but got: " << value << std::endl;
		return 1;
	}
}
