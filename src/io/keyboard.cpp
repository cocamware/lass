/**     @file
 *      @author Bram de Greve (bram@cocamware.com)
 *      @author Tom De Muer (tom@cocamware.com)
 *
 *      *** BEGIN LICENSE INFORMATION ***
 *      
 *      The contents of this file are subject to the Common Public Attribution License 
 *      Version 1.0 (the "License"); you may not use this file except in compliance with 
 *      the License. You may obtain a copy of the License at 
 *      http://lass.sourceforge.net/cpal-license. The License is based on the 
 *      Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover 
 *      use of software over a computer network and provide for limited attribution for 
 *      the Original Developer. In addition, Exhibit A has been modified to be consistent 
 *      with Exhibit B.
 *      
 *      Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
 *      WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
 *      language governing rights and limitations under the License.
 *      
 *      The Original Code is LASS - Library of Assembled Shared Sources.
 *      
 *      The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *      The Original Developer is the Initial Developer.
 *      
 *      All portions of the code written by the Initial Developer are:
 *      Copyright (C) 2004-2009 the Initial Developer.
 *      All Rights Reserved.
 *      
 *      Contributor(s):
 *
 *      Alternatively, the contents of this file may be used under the terms of the 
 *      GNU General Public License Version 2 or later (the GPL), in which case the 
 *      provisions of GPL are applicable instead of those above.  If you wish to allow use
 *      of your version of this file only under the terms of the GPL and not to allow 
 *      others to use your version of this file under the CPAL, indicate your decision by 
 *      deleting the provisions above and replace them with the notice and other 
 *      provisions required by the GPL License. If you do not delete the provisions above,
 *      a recipient may use your version of this file under either the CPAL or the GPL.
 *      
 *      *** END LICENSE INFORMATION ***
 */

#include "lass_common.h"
#include "keyboard.h"

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#       pragma LASS_NOTE("io/keyboard: using win32 implementation")
#       define LASS_IO_KEYBOARD_HAVE_WIN32
#       include <conio.h>
#elif LASS_HAVE_TERMIOS_H && LASS_HAVE_SYS_IOCTL_H
#       define LASS_IO_KEYBOARD_HAVE_TERMIOS_H_AND_IOCTL_H
#       include <termios.h>
#       include <sys/ioctl.h>
#       if LASS_HAVE_UNISTD_H
#               include <unistd.h>
#       endif
#       if LASS_HAVE_SYS_SOCKET_H
#               include <sys/socket.h>
#       endif
#       include "../util/impl/lass_errno.h"
#else
#       error lass/io/keyboard.cpp not supported
#endif

#
namespace lass
{
namespace io
{

#if defined(LASS_IO_KEYBOARD_HAVE_TERMIOS_H_AND_IOCTL_H)
namespace impl
{
        class TerminalAttributeSaver
        {
        public:
                TerminalAttributeSaver()
                {
                        LASS_ENFORCE_CLIB(tcgetattr(STDIN_FILENO, &savedAttributes_));
                }
                ~TerminalAttributeSaver()
                {
                        const int rc = tcsetattr(STDIN_FILENO, TCSANOW, &savedAttributes_);
                        LASS_ASSERT(rc == 0);
                        if (rc == -1)
                        {
                                const int errnum = util::impl::lass_errno();
                                std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: "
                                        << "failed to restore terminal attributes: ("
                                        << errnum << ") " << util::impl::lass_strerror(errnum)
                                        << std::endl;
                        }
                }
                const termios& savedAttributes() const
                {
                        return savedAttributes_;
                }
        private:
                termios savedAttributes_;
        };
}
#endif

/** return true if a key is pressed and which is waiting in the buffer ...
 *  @ingroup Keyboard
 */
bool keyboardIsHit()
{
#if defined(LASS_IO_KEYBOARD_HAVE_WIN32)
        return _kbhit() != 0;
#elif defined(LASS_IO_KEYBOARD_HAVE_TERMIOS_H_AND_IOCTL_H)
        impl::TerminalAttributeSaver attributeSaver;
        termios rawMode = attributeSaver.savedAttributes();
        rawMode.c_lflag &= ~ICANON;
        LASS_ENFORCE_CLIB(tcsetattr(STDIN_FILENO, TCSANOW, &rawMode));

        int count = 0;
        LASS_ENFORCE_CLIB(ioctl(STDIN_FILENO, FIONREAD, &count));
        return count > 0;
#else
#       error keyboardIsHit not supported
#endif
}

/** get a character from the console without echo
 *  @ingroup Keyboard
 */
int keyboardGetKey()
{
#if defined(LASS_IO_KEYBOARD_HAVE_WIN32)
        return _getch();
#elif defined(LASS_IO_KEYBOARD_HAVE_TERMIOS_H_AND_IOCTL_H)
        impl::TerminalAttributeSaver attributeSaver;
        termios rawMode = attributeSaver.savedAttributes();
        rawMode.c_lflag &= ~ICANON;
        rawMode.c_lflag &= ~ECHO;
        rawMode.c_cc[VMIN]  = 1; // block for input
        rawMode.c_cc[VTIME] = 0; // timer is ignored
        LASS_ENFORCE_CLIB(tcsetattr(STDIN_FILENO, TCSANOW, &rawMode));

        unsigned char character = 0;
        const ssize_t charactersRead = LASS_ENFORCE_CLIB(read(STDIN_FILENO, &character, 1));
        LASS_ENFORCE(charactersRead == 1);
        return static_cast<int>(character);
#else
#       error keyboardIsHit not supported
#endif
}

}
}

// EOF
