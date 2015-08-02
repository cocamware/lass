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
		LASS_META_ASSERT(sizeof(T) <= payloadSize_, value_is_to_big_for_payload);
		*reinterpret_cast<T*>(payload_) = value;
	}

	template <typename T> T value() const
	{
		LASS_META_ASSERT(sizeof(T) <= payloadSize_, value_is_to_big_for_payload);
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
