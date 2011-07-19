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

#ifndef _CORE_FILESYSTEM_HPP
#define _CORE_FILESYSTEM_HPP

#include <string>
#include <vector>

namespace core {
	class Path {
		std::vector< std::string > path_;
		bool absolute_;

	public:
		Path(std::string const path = std::string());

		void set(std::string const path = std::string());

		bool absolute() const;
		bool exists() const;

		std::string toString() const;

		static std::string data();
		static std::string home();

		static bool exists(std::string const path);
	};
}

#endif
