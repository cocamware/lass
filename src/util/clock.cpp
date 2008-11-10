/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#include "lass_common.h"
#include "clock.h"

namespace lass
{
namespace util
{

Clock::Clock(TTime iStartTime):
	frequency_(impl::ClockImpl::frequency())
{
	resolution_ = num::NumTraits<TTime>::one / frequency_;
	reset(iStartTime);
}



void Clock::reset(TTime iStartTime)
{
	startTick_ = impl::ClockImpl::tick();
	startTime_ = iStartTime;
}


const std::string Clock::humanize(const TTime& iTime)
{
	std::ostringstream buffer;
	buffer.copyfmt(std::cout);
	buffer.setf(std::ios_base::showpoint);
	buffer.setf(static_cast<std::ios_base::fmtflags>(0), std::ios_base::floatfield);
	buffer.precision(3);
	//buffer << std::fixed << std::setprecision(3) << std::showpoint;

	if (iTime < 1e-9)
	{
		buffer << (1e12 * iTime) << "ps";
	}
	else if (iTime < 1e-6)
	{
		buffer << (1e9 * iTime) << "ns";
	}
	else if (iTime < 1e-3)
	{
		buffer << (1e6 * iTime) << "us";
	}
	else if (iTime < 1.0)
	{
		buffer << (1e3 * iTime) << "ms";
	}
	else if (iTime < 60)
	{
		buffer << iTime << "s";
	}
	else if (iTime < 3600)
	{
		buffer << (iTime / 60) << "m";
	}
	else if (iTime < 86400)
	{
		buffer << (iTime / 3600) << "h";
	}
	else
	{
		buffer << (iTime / 86400) << "d";
	}

	return buffer.str();
}



}

}

// EOF
