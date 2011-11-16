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

#include "lass_common.h"
#include "progress_indicator.h"
#include "../num/basic_ops.h"

namespace lass
{
namespace util
{

/** @param iDescriptor description printed at begin of the line
 *  @param iConsoleWidth maximum number of characters on one console text line (default=80)
 */
ProgressIndicator::ProgressIndicator(const std::string& iDescription, int iConsoleWidth):
	avgStepDuration_(0.f),
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
		if (current_ != 6666)
		{
			std::ostringstream stepInfo;
			stepInfo.copyfmt(std::cout);
			stepInfo << description_ << ": 100%; CPU " << Clock::humanize(clock_.time()) 
				<< whitespace_;
			std::cout << "\r" << stepInfo.str().substr(0, consoleWidth_ - 1) << std::endl;
			current_ = 6666;
		}
		return;
	}
		
	iProgress = num::clamp(iProgress, num::NumTraits<double>::zero, num::NumTraits<double>::one);
	const int promille = static_cast<int>(1000 * iProgress);

	const Clock::TTime updateRate = 0.025f;

	if (promille > current_)
	{
		const Clock::TTime timeElapsed = clock_.time();
		const Clock::TTime stepDuration = (timeElapsed - previousTimeElapsed_) / (promille - current_);
		if (promille < 5)
		{
			avgStepDuration_ = stepDuration;
		}
		else
		{
			avgStepDuration_ = num::lerp(avgStepDuration_, stepDuration, updateRate);
		}
		const Clock::TTime timeLeft = (1000 - promille) * avgStepDuration_;

		std::ostringstream stepInfo;
		stepInfo.copyfmt(std::cout);
		stepInfo << description_ << ": " << std::fixed << std::setprecision(1) << (.1 * promille) 
			<< "%; CPU " << Clock::humanize(timeElapsed)
			<< " [-" << Clock::humanize(timeLeft) << "]" << whitespace_;
		std::cout << "\r" << stepInfo.str().substr(0, consoleWidth_ - 1) << std::flush;

		current_ = promille;
		previousTimeElapsed_ = timeElapsed;
	}
}

}

}

// EOF

