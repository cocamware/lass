#include <string>
#include <lass/python/python_api.h>

class Foo
{
public:
	Foo() {}
	virtual ~Foo() {}

	int publicMember;

	int getMember() { return publicMember; }
	void setMember(int iMember) { publicMember=iMember; }
	
	int coolMember() const { return publicMember; }
	int& coolMember() { return publicMember; }

	int overloaded(int iA) { return iA;}
	int overloaded(int iA, int iB) { return iA+iB;}
	int overloaded(int iA, int iB, int iC) { return iA+iB+iC;}

	virtual std::string virtualFunction() { return std::string("Foo"); }

	Foo(int iX, int iY = 5) {}
};

PY_SHADOW_CLASS( LASS_DLL_EXPORT, PyFoo, Foo )
PY_DECLARE_CLASS_NAME( PyFoo, "Foo" )
PY_SHADOW_DOWN_CASTERS( PyFoo )

	PY_CLASS_CONSTRUCTOR_0( PyFoo )
	PY_CLASS_CONSTRUCTOR_1( PyFoo, int  )
	PY_CLASS_CONSTRUCTOR_2( PyFoo, int, int )

	PY_CLASS_PUBLIC_MEMBER( PyFoo, publicMember )
	PY_CLASS_PUBLIC_MEMBER_R_NAME( PyFoo, publicMember, "readOnlyPublicMember" )

	PY_CLASS_METHOD( PyFoo, getMember )
	PY_CLASS_METHOD( PyFoo, setMember )
	//PY_CLASS_MEMBER_RW_NAME( PyFoo, getMember, setMember, "accessorMember" );

	PY_CLASS_MEMBER_RW_NAME( PyFoo, coolMember, coolMember, "cool" );

	PY_CLASS_METHOD_QUALIFIED_1( PyFoo, overloaded, int, int )
	PY_CLASS_METHOD_QUALIFIED_2( PyFoo, overloaded, int, int, int )
	PY_CLASS_METHOD_QUALIFIED_3( PyFoo, overloaded, int, int, int, int )

PY_DECLARE_MODULE( embedding )
void injectEmbeddingClasses()
{
	PY_INJECT_CLASS_IN_MODULE( PyFoo, embedding, "")
}
PY_EXTENSION_MODULE( embedding, injectEmbeddingClasses )
