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
 *	Copyright (C) 2015-2025 the Initial Developer.
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

#ifndef GUARDIAN_OF_INCLUSION_IPC_PROTOCOL_H
#define GUARDIAN_OF_INCLUSION_IPC_PROTOCOL_H

#include <lass/num/basic_types.h>
#include <lass/stde/extended_cstring.h>

namespace ipc
{

enum MessageCode
{
	mcNone, // nothing to say, message-less message
	mcHello, // child -> parent, sent just after it succesfully connected.
	mcDouble, // parent -> child: double a number; payload: 32-bit float
	mcDoubled, // child -> parent: the doubled number; payload: 32-bit float
	mcUpper,
	mcUppered,
	mcExit, // parent -> child: when child needs to exit
	mcGoodbye, // child -> parent, just before it exits (in response to mcExit)
	mcError,
};


typedef lass::num::Tuint32 Tuint32;
typedef lass::num::Tfloat32 Tfloat32;


class Message
{
public:
	Message(MessageCode code = mcNone):
		code_(code)
	{
		::memset(payload_, 0, sizeof(payload_));
	}

	template <typename T>
	Message(MessageCode code, const T& value) :
		code_(code)
	{
		::memset(payload_, 0, sizeof(payload_));
		setValue(value);
	}

	MessageCode code() const
	{
		return static_cast<MessageCode>(code_);
	}
	const char* payload() const
	{
		return payload_;
	}
	char* payload()
	{
		return payload_;
	}

	void setValue(const char* str)
	{
		lass::stde::safe_strcpy(payload_, str);
	}
	template <typename T> void setValue(const T& value)
	{
		static_assert(sizeof(T) <= payloadSize_, "value must fit in payload");
		*reinterpret_cast<T*>(payload_) = value;
	}

	template <typename T> T value() const
	{
		static_assert(sizeof(T) <= payloadSize_, "value must fit in payload");
		return *reinterpret_cast<const T*>(payload_);
	}

private:
	enum
	{
		payloadSize_ = 60,
	};

	Tuint32 code_;
	char payload_[payloadSize_];
};



}

#endif
