/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#include "num_common.h"
#include "num_traits.h"

namespace lass
{
	namespace num
	{
		
		/** p2dB.  Converts a pressure into decibels */ 
		template< typename T >	T	p2dB( const T& iValue );

		/** W2dB.  Converts a power into decibels */ 
		template< typename T >	T	W2dB( const T& iValue );

		/** I2dB.  Converts an intensity into decibels.  The intensity represents the intensity 
		*	in all directions expressed in Watts/m^2, referenced to 10^-12 */ 
		template< typename T >	T	I2dB( const T& iValue );

		/** dB2p.  Converts decibels into a pressure */ 
		template< typename T >	T	dB2p( const T& iValue );

		/** dB2W.  Converts decibels into a power */ 
		template< typename T >	T	db2W( const T& iValue );

		/** dB2I.  Converts decibels into an intensity.  The intensity represents the intensity 
		*	in all directions expressed in Watts/m^2, referenced to 10^-12 */ 
		template< typename T >	T	db2I( const T& iValue );
	}
}

#include "db.inl"
