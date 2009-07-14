#include <string>
#include <lass/python/python_api.h>

class Foo
{
public:
	Foo() {}
	virtual ~Foo() {}
	virtual int overloaded(int iA) { return iA+1;}
	int fooSpecific() { return -1; }
};

class Bar : public Foo
{
public:
	Bar() {}
	virtual ~Bar() {}
	virtual int overloaded(int iA) { return iA+2;}
	int barSpecific() { return -2; }
};

class FooBarred : public Bar
{
public:
	FooBarred() {}
	virtual ~FooBarred() {}
	virtual int overloaded(int iA) { return iA+3;}
	int fooBarSpecific() { return -3; }
};

lass::util::SharedPtr<Foo> factoryObject(int iWhich)
{
	if (iWhich==3)
		return lass::util::SharedPtr<Foo>(new FooBarred());
	if (iWhich==2)
		return lass::util::SharedPtr<Foo>(new Bar());
	return lass::util::SharedPtr<Foo>(new Foo());
}

struct Containers
{
	static std::vector<lass::util::SharedPtr<Foo> > aListOfFoos()
	{	
		std::vector<lass::util::SharedPtr<Foo> > temp;
		temp.push_back(factoryObject(1));
		temp.push_back(factoryObject(2));
		temp.push_back(factoryObject(3));
		return temp;
	}
	static const std::vector<lass::util::SharedPtr<Foo> >& aConstListOfFoos()
	{	
		static std::vector<lass::util::SharedPtr<Foo> > temp;
		if (temp.size()==0)
		{
			temp.push_back(factoryObject(1));
			temp.push_back(factoryObject(2));
			temp.push_back(factoryObject(3));
		}
		return temp;
	}
	static std::map<int,lass::util::SharedPtr<Foo> > aMapOfFoos()
	{	
		std::map<int,lass::util::SharedPtr<Foo> > temp;
		temp.insert(std::pair<int,lass::util::SharedPtr<Foo> >(1,factoryObject(1)));
		temp.insert(std::pair<int,lass::util::SharedPtr<Foo> >(2,factoryObject(2)));
		temp.insert(std::pair<int,lass::util::SharedPtr<Foo> >(3,factoryObject(3)));
		return temp;
	}
	static int doSum(const std::vector<int>& iVector)
	{
		return std::accumulate(iVector.begin(),iVector.end(),0);
	}
};


PY_SHADOW_CLASS( LASS_DLL_EXPORT, PyFoo, Foo )
PY_DECLARE_CLASS_NAME( PyFoo, "Foo" )
PY_SHADOW_DOWN_CASTERS( PyFoo )
	PY_CLASS_METHOD( PyFoo, overloaded )
	PY_CLASS_METHOD( PyFoo, fooSpecific )

PY_SHADOW_CLASS_DERIVED( LASS_DLL_EXPORT, PyBar, Bar, PyFoo)
PY_DECLARE_CLASS_NAME( PyBar, "Bar" )
PY_SHADOW_DOWN_CASTERS( PyBar )
	PY_CLASS_METHOD( PyBar, barSpecific )

PY_SHADOW_CLASS_DERIVED( LASS_DLL_EXPORT, PyFooBarred, FooBarred, PyBar)
PY_DECLARE_CLASS_NAME( PyFooBarred, "FooBarred" )
PY_SHADOW_DOWN_CASTERS( PyFooBarred )
	PY_CLASS_METHOD( PyFooBarred, fooBarSpecific )

PY_DECLARE_MODULE( embedding )
	PY_MODULE_FUNCTION( embedding, factoryObject )

PY_MODULE_FUNCTION_NAME( embedding, Containers::aListOfFoos, "aListOfFoos" )
PY_MODULE_FUNCTION_NAME( embedding, Containers::aConstListOfFoos, "aConstListOfFoos" )
PY_MODULE_FUNCTION_NAME( embedding, Containers::aMapOfFoos, "aMapOfFoos" )
PY_MODULE_FUNCTION_NAME( embedding, Containers::doSum, "doSum" )

void injectEmbeddingClasses()
{
	PY_INJECT_CLASS_IN_MODULE( PyFoo, embedding, "")
	PY_INJECT_CLASS_IN_MODULE( PyBar, embedding, "")
	PY_INJECT_CLASS_IN_MODULE( PyFooBarred, embedding, "")
}
PY_EXTENSION_MODULE( embedding, injectEmbeddingClasses )
