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

#include <core/filesystem.hpp>

namespace test { namespace filesystem {
/*
	Test creating paths
*/
	void strangePaths() {
		::core::Path empty_path;
		equal(empty_path.toString(), "/");

		::core::Path ordinary_path("/an/ordinary/path");
		equal(ordinary_path.toString(), "/an/ordinary/path");

		::core::Path extra_separators("/path//with///extra/separators//");
		equal(extra_separators.toString(), "/path/with/extra/separators");

		::core::Path with_dot("/a/path/with/./dot");
		equal(with_dot.toString(), "/a/path/with/dot");

		::core::Path with_dots("/a/path/with/../dots");
		equal(with_dots.toString(), "/a/path/dots");
	}
	void runTests() {
		std::cout << "Detected home directory: " << ::core::Path::home() << std::endl;
		std::cout << "Application data directory: " << ::core::Path::data() << std::endl;

		strangePaths();
	}
}}
