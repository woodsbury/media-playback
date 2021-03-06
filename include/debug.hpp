/*
	Copyright (C) 2011  Wade Smith

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _DEBUG_HPP
#define _DEBUG_HPP

#if defined(DEBUG)

/*
Control debug output from within the application
*/
void dprint_enable(bool enabled);

/*
Prints a message to the console if DEBUG is defined when compiled
*/
void dprint(char const * format, ...);

#else

#define dprint_enable(x) static_cast<void>(0)
#define dprint(...) static_cast<void>(0)
#define NDEBUG

#endif

#include <cassert>

#endif
