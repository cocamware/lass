#include <lass/python/python_api.h>
#include <iostream>

int main()
{
	using namespace lass;

	Py_Initialize();

	python::TPyObjPtr mod(PyImport_ImportModule("test_module"));
	if (!mod)
	{
		return 1;
	}
	python::TPyObjPtr multiply(PyObject_GetAttrString(mod.get(), "multiply"));
	if (!multiply)
	{
		return 1;
	}

	auto args = python::makeTuple(6, 0.5);
	python::TPyObjPtr obj(PyObject_CallObject(multiply.get(), args.get()));
	if (!obj)
	{
		return 1;
	}
	double value;
	if (python::pyGetSimpleObject(obj.get(), value) != 0)
	{
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
