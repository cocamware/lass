#include "bulk_add.h"

namespace lass
{
namespace python
{
namespace impl
{

std::string argument(const std::string& iValue, int iArg)
{
	std::vector<std::string> splits( lass::stde::split( iValue, "," ) );
	return lass::stde::strip(splits[iArg],std::string(" "));
}

void addIntegerConstantToModule( PyObject* iModule, long iValue, const std::string& iName )
{
	LASS_ENFORCE( iModule );
	PyModule_AddIntConstant( iModule, iName.c_str(), iValue);
}

}

}

}	// namespace lass

