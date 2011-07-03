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
#include <cstdarg>
#include <cstdio>

namespace {
/*
	Prints a message to the console if DEBUG is defined when compiled
*/
	void dprint(char const * format, ...) {
		std::va_list args;
		va_start(args, format);
		std::vprintf(format, args);
		std::printf("\n");
		va_end(args);
	}
}

#else
#define dprint(...)
#endif

#endif
