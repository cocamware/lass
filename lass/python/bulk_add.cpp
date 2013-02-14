#include "python_common.h"
#include "bulk_add.h"

namespace lass
{
namespace python
{
namespace impl
{

std::string argument(const std::string& iValue, size_t iArg)
{
	std::vector<std::string> splits( lass::stde::split( iValue, "," ) );
	return lass::stde::strip(splits[iArg],std::string(" "));
}

}

}

}	// namespace lass

