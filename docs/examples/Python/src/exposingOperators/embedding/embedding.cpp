#include <lass/python/python_api.h>

struct SpecialNumber
{
	SpecialNumber() : r_(0), i_(0) {}
	SpecialNumber(float r) : r_(r), i_(0) {}
	SpecialNumber(float r,float i) : r_(r), i_(i) {}
	float real() const { return r_; }
	float imag() const { return i_; }
	void real(float iV) { r_ = iV; }
	void imag(float iV) { i_ = iV; }

	SpecialNumber& operator+=(float iV) { r_ += iV; return *this;}
	SpecialNumber& operator+=(const SpecialNumber& iV) { r_ += iV.r_; i_ += iV.i_; return *this;}
	std::string repr() const { std::stringstream tmp; tmp << "(" << r_ << (i_<0?"-":"+") << "j" << fabsf(i_) << ")"; return tmp.str(); }
protected:
	float i_;
	float r_;
};

SpecialNumber operator+(const SpecialNumber& iV, const SpecialNumber& iV2)
{
	SpecialNumber temp(iV);
	temp += iV2;
	return temp;
}

SpecialNumber operator+(const SpecialNumber& iV, float iV2)
{
	SpecialNumber temp(iV);
	temp += iV2;
	return temp;
}



PY_SHADOW_CLASS( LASS_DLL_EXPORT, PySpecialNumber, SpecialNumber )
PY_DECLARE_CLASS_NAME( PySpecialNumber, "SpecialNumber" )
PY_SHADOW_DOWN_CASTERS( PySpecialNumber)
	PY_CLASS_CONSTRUCTOR_0( PySpecialNumber )
	PY_CLASS_CONSTRUCTOR_1( PySpecialNumber, float )
	PY_CLASS_CONSTRUCTOR_2( PySpecialNumber, float, float )
	PY_CLASS_MEMBER_RW( PySpecialNumber, real, real)
	PY_CLASS_MEMBER_RW( PySpecialNumber, imag, imag)
	PY_CLASS_METHOD_QUALIFIED_NAME_1( PySpecialNumber, operator+=, SpecialNumber&, float, lass::python::methods::_iadd_ )
	PY_CLASS_METHOD_QUALIFIED_NAME_1( PySpecialNumber, operator+=, SpecialNumber&, const SpecialNumber&, lass::python::methods::_iadd_ )

	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_2( PySpecialNumber, operator+, SpecialNumber, const SpecialNumber&, const SpecialNumber&, lass::python::methods::_add_ )
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_2( PySpecialNumber, operator+, SpecialNumber, const SpecialNumber&, float, lass::python::methods::_add_ )
	PY_CLASS_METHOD_NAME( PySpecialNumber, repr, lass::python::methods::_repr_ )
	PY_CLASS_METHOD_NAME( PySpecialNumber, repr, lass::python::methods::_str_ )


PY_DECLARE_MODULE( embedding )
void injectEmbeddingClasses()
{
	PY_INJECT_CLASS_IN_MODULE( PySpecialNumber, embedding, "")
}
PY_EXTENSION_MODULE( embedding, injectEmbeddingClasses )
