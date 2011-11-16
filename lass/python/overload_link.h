/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
 *
 *	*** BEGIN LICENSE INFORMATION ***
 *	
 *	The contents of this file are subject to the Common Public Attribution License 
 *	Version 1.0 (the "License"); you may not use this file except in compliance with 
 *	the License. You may obtain a copy of the License at 
 *	http://lass.sourceforge.net/cpal-license. The License is based on the 
 *	Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover 
 *	use of software over a computer network and provide for limited attribution for 
 *	the Original Developer. In addition, Exhibit A has been modified to be consistent 
 *	with Exhibit B.
 *	
 *	Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
 *	WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
 *	language governing rights and limitations under the License.
 *	
 *	The Original Code is LASS - Library of Assembled Shared Sources.
 *	
 *	The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *	The Original Developer is the Initial Developer.
 *	
 *	All portions of the code written by the Initial Developer are:
 *	Copyright (C) 2004-2011 the Initial Developer.
 *	All Rights Reserved.
 *	
 *	Contributor(s):
 *
 *	Alternatively, the contents of this file may be used under the terms of the 
 *	GNU General Public License Version 2 or later (the GPL), in which case the 
 *	provisions of GPL are applicable instead of those above.  If you wish to allow use
 *	of your version of this file only under the terms of the GPL and not to allow 
 *	others to use your version of this file under the CPAL, indicate your decision by 
 *	deleting the provisions above and replace them with the notice and other 
 *	provisions required by the GPL License. If you do not delete the provisions above,
 *	a recipient may use your version of this file under either the CPAL or the GPL.
 *	
 *	*** END LICENSE INFORMATION ***
 */

/** @defgroup Python
 *  @brief interface library to Python
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_OVERLOAD_LINK_H
#define LASS_GUARDIAN_OF_INCLUSION_PYTHON_OVERLOAD_LINK_H

#include "python_common.h"

namespace lass
{
	namespace python
	{
		namespace impl
		{
			/**	@ingroup Python
			 *	@internal
			 */
			class LASS_PYTHON_DLL OverloadLink
			{
			public:
				enum Signature
				{
					sNull,
					sPyCFunction,
					sBinary,
					sTernary,
					sSsizeArg,
					sSsizeSsizeArg,
					sSsizeObjArg,
					sSsizeSsizeObjArg,
					sObjObj,
					sObjObjArg,
					sArgKw
				};
				OverloadLink();
				void setNull();
				void setPyCFunction(PyCFunction iOverload);
				void setBinaryfunc(binaryfunc iOverload);
				void setTernaryfunc(ternaryfunc iOverload);
				
				void setSsizeArgfunc(ssizeargfunc iOverload);
				void setSsizeSsizeArgfunc(ssizessizeargfunc iOverload);
				void setSsizeObjArgProcfunc(ssizeobjargproc iOverload);
				void setSsizeSsizeObjArgProcfunc(ssizessizeobjargproc iOverload);
				void setObjObjProcfunc(objobjproc iOverload);
				void setObjObjArgProcfunc(objobjargproc iOverload);

				void setArgKwfunc(ternaryfunc iOverload);

				bool operator()(PyObject* iSelf, PyObject* iArgs, 
					PyObject*& result) const;
			private:
				PyObject* call(PyObject* iSelf, PyObject* iArgs) const;
				union
				{
					PyCFunction pyCFunction_;
					binaryfunc binaryfunc_;
					ternaryfunc ternaryfunc_;

					ssizeargfunc ssizeargfunc_;
					ssizessizeargfunc ssizessizeargfunc_;
					ssizeobjargproc ssizeobjargproc_;
					ssizessizeobjargproc ssizessizeobjargproc_;
					objobjproc objobjproc_;
					objobjargproc objobjargproc_;

					getiterfunc getiterfunc_;
					iternextfunc iternextfunc_;
				};
				Signature signature_;
			};
		}
	}
}

#endif
