#include <string>
#include <lass/python/python_api.h>

std::string hello()
{
	return std::string("Hello world!");
}

PY_DECLARE_MODULE( embedding )
PY_MODULE_FUNCTION( embedding, hello )

void injectEmbeddingClasses() { }

PY_EXTENSION_MODULE( embedding, injectEmbeddingClasses )
