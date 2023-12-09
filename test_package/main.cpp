#include <lass/python/python_api.h>
#include <iostream>

int main()
{
	using namespace lass;

	Py_Initialize();
	python::TPyObjPtr x = python::evaluate("4 + 4");
	int y = 0;
	int z = python::pyGetSimpleObject(x.get(), y);
	if (z == 0 && y == 8)
	{
		std::cout << "OK\n";
		return 0;
	}
	return 1;
}
