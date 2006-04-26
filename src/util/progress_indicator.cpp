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
#include "progress_indicator.h"

namespace lass
{
namespace util
{

/** @param iDescriptor description printed at begin of the line
 *  @param iConsoleWidth maximum number of characters on one console text line (default=80)
 */
ProgressIndicator::ProgressIndicator(const std::string& iDescription, int iConsoleWidth):
	timeLeftBuffer_(0.f),
	previousTimeElapsed_(0.f),
	description_(iDescription),
	whitespace_(iConsoleWidth, ' '),
	consoleWidth_(iConsoleWidth),
	current_(0),
	timeLeftIndex_(0)
{
	std::ostringstream stepInfo;
	stepInfo.copyfmt(std::cout);
	stepInfo << description_ << ": 0%; CPU 0s" << whitespace_;
	std::cout << stepInfo.str().substr(0, consoleWidth_ - 1) << std::flush;
}

/** print final message on destruction
 */
ProgressIndicator::~ProgressIndicator()
{
	(*this)(num::NumTraits<double>::one);
}

/** update progress indicator
 *  @param iProgress value in range [0, 1]
 */
void ProgressIndicator::operator()(double iProgress)
{
	if (iProgress >= num::NumTraits<double>::one)
	{
		// final message
		if (current_ != 666)
		{
			std::ostringstream stepInfo;
			stepInfo.copyfmt(std::cout);
			stepInfo << description_ << ": 100%; CPU " << Clock::humanize(clock_.time()) 
				<< whitespace_;
			std::cout << "\r" << stepInfo.str().substr(0, consoleWidth_ - 1) << std::endl;
			current_ = 666;
		}
		return;
	}
		
	iProgress = num::clamp(iProgress, num::NumTraits<double>::zero, num::NumTraits<double>::one);
	const int procent = static_cast<int>(100 * iProgress);

	const Clock::TTime updateRate = 0.25f;

	if (procent > current_)
	{
		const Clock::TTime timeElapsed = clock_.time();
		const Clock::TTime stepDuration = timeElapsed - previousTimeElapsed_;
		const Clock::TTime timeLeft = (100 - procent) * stepDuration;
		if (procent == 1)
		{
			timeLeftBuffer_ = timeLeft;
		}
		else
		{
			timeLeftBuffer_ += updateRate * (timeLeft - timeLeftBuffer_);
		}

		std::ostringstream stepInfo;
		stepInfo.copyfmt(std::cout);
		stepInfo << description_ << ": " << procent << "%; CPU " << Clock::humanize(timeElapsed)
			<< " [" << Clock::humanize(timeLeftBuffer_) << "]" << whitespace_;
		std::cout << "\r" << stepInfo.str().substr(0, consoleWidth_ - 1) << std::flush;
		current_ = procent;

		previousTimeElapsed_ = timeElapsed;
	}
}

}

}

// EOF

