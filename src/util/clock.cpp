/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "util_common.h"
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
