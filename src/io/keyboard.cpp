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

#include "io_common.h"
#include "keyboard.h"

#if HAVE_CONFIG_H
#	include "../../lass_auto_config.h"
#endif

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#	pragma LASS_NOTE("io/keyboard: using win32 implementation")
#	define LASS_IO_KEYBOARD_HAVE_WIN32
#	include <conio.h>
#elif HAVE_TERMIOS_H && HAVE_SYS_IOCTL_H
#	define LASS_IO_KEYBOARD_HAVE_TERMIOS_H_AND_IOCTL_H
#	include <termios.h>
#	include <sys/ioctl.h>
#	include "../util/impl/lass_errno.h"
#else
#	error lass/io/keyboard.cpp not supported
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
#	error keyboardIsHit not supported
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
#	error keyboardIsHit not supported
#endif
}

}
}

// EOF
