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

#ifndef _CORE_NONCOPIABLE_HPP
#define _CORE_NONCOPIABLE_HPP

namespace core {
	/*
		Inheriting from this class will prevent copying
	*/
	class NonCopiable {
		NonCopiable(NonCopiable const &) = delete;
		NonCopiable const & operator=(NonCopiable const &) = delete;

	protected:
		NonCopiable() = default;
	};
}

#endif
