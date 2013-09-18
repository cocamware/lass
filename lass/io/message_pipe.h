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
 *	Copyright (C) 2004-2013 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_MESSAGE_PIPE_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_MESSAGE_PIPE_H

#include "io_common.h"

namespace lass
{
namespace io
{
namespace impl
{
    class MessagePipeImpl;
}

/** @brief A bidirectional pipe to send message back and forth between two processes.
 *  @author Bramz
 *  @date 2013
 *
 *  On windows this is a named pipe, on linux this is a local socket with an abstract name.
 */
class LASS_DLL MessagePipe
{
public:
    enum { infinite = size_t(-1) };
    
    MessagePipe(size_t maxMessageSize = 0);
    ~MessagePipe();
    
    bool create();    
    bool connect(const char* pipeName, size_t msecTimeout=infinite);
    bool accept(size_t msecTimeout=infinite);
    void close();
    
    const char* name() const;
    bool operator!() const;

    bool send(const void* out, size_t size, size_t msecTimeout=infinite) const;
    bool receive(void* in, size_t size, size_t msecTimeout=infinite) const;
    bool transact(const void* out, size_t outSize, void* in, size_t inSize, size_t msecTimeout=infinite) const;

private:
    MessagePipe(const MessagePipe&);
    MessagePipe& operator=(const MessagePipe&);
    impl::MessagePipeImpl* pimpl_;
};


template <typename MessageType>
class TypedMessagePipe
{
public:
    typedef MessageType TMessage;
    enum { infinite = MessagePipe::infinite };
    
    TypedMessagePipe(): pipe_(sizeof(TMessage)) {}    
    
    bool create() { return pipe_.create(); }
    bool connect(const char* pipeName, size_t msecTimeout=infinite) { return pipe_.connect(pipeName, msecTimeout); }
    bool accept(size_t msecTimeout=infinite) { return pipe_.accept(msecTimeout); }
    void close() { pipe_.close(); }
    
    const char* name() const { return pipe_.name(); }
    bool operator!() const { return !pipe_; }

    bool send(const TMessage& out, size_t msecTimeout=infinite) const { return pipe_.send(&out, sizeof(TMessage), msecTimeout); }
    bool receive(TMessage& in, size_t msecTimeout=infinite) const { return pipe_.receive(&in, sizeof(TMessage), msecTimeout); }
    bool transact(const TMessage& out, TMessage& in, size_t msecTimeout=infinite) const { return pipe_.transact(&out, sizeof(TMessage), &in, sizeof(TMessage), msecTimeout); }

private:
    MessagePipe pipe_;
};

}
}

#endif
