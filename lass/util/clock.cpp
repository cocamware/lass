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
 *	Copyright (C) 2004-2010 the Initial Developer.
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
	resolution_ = num::NumTraits<TTime>::one / static_cast<TTime>(frequency_);
	reset(iStartTime);
}



void Clock::reset(TTime iStartTime)
{
	startTick_ = impl::ClockImpl::tick();
	startTime_ = iStartTime;
}



const std::string Clock::humanize(const TTime& time)
{
	std::ostringstream buffer;
	buffer.copyfmt(std::cout);

	if (time < 60)
	{
		buffer.setf(std::ios_base::showpoint);
		buffer.setf(static_cast<std::ios_base::fmtflags>(0), std::ios_base::floatfield);
		buffer.precision(3);
		if (time < 1e-9)
		{
			buffer << (1e12 * time) << "ps";
		}
		else if (time < 1e-6)
		{
			buffer << (1e9 * time) << "ns";
		}
		else if (time < 1e-3)
		{
			buffer << (1e6 * time) << "us";
		}
		else if (time < 1.0)
		{
			buffer << (1e3 * time) << "ms";
		}
		else
		{
			buffer << time << "s";
		}
	}
	else
	{
		struct Helper
		{
			static void write(std::ostringstream& stream, TTime time, const std::string& bigUnit, TTime SmallInBig, const std::string& smallUnit)
			{
				const TTime sign = num::sign(time);
				const TTime absTime = num::abs(time);
				const TTime bigHand = num::floor(absTime);
				const TTime smallHand = num::floor(SmallInBig * (absTime - bigHand));
				stream << static_cast<int>(sign * bigHand) << bigUnit << std::setw(2) << std::setfill('0') << static_cast<int>(smallHand) << smallUnit;
			}
		};

		if (time < 3600)
		{
			Helper::write(buffer, time / 60, "m", 60, "s");
		}
		else if (time < 86400)
		{
			Helper::write(buffer, time / 3600, "h", 60, "m");
		}
		else
		{
			Helper::write(buffer, time / 86400, "d", 24, "h");
		}
	}

	return buffer.str();
}



}

}

// EOF
