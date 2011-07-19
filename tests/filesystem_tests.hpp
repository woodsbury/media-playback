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
		isTrue(empty_path.absolute());

		::core::Path ordinary_path("/an/ordinary/path");
		equal(ordinary_path.toString(), "/an/ordinary/path");
		isTrue(ordinary_path.absolute());

		::core::Path extra_separators("/path//with///extra/separators//");
		equal(extra_separators.toString(), "/path/with/extra/separators");
		isTrue(extra_separators.absolute());

		::core::Path with_dot("/a/path/with/./dot");
		equal(with_dot.toString(), "/a/path/with/dot");
		isTrue(with_dot.absolute());

		::core::Path with_dots("/a/path/with/../dots");
		equal(with_dots.toString(), "/a/path/dots");
		isTrue(with_dots.absolute());

		::core::Path relative_path("relative/path");
		equal(relative_path.toString(), "relative/path");
		isFalse(relative_path.absolute());

		relative_path.set("new/path/");
		equal(relative_path.toString(), "new/path");
		isFalse(relative_path.absolute());
	}

/*
	Test for path checking
*/
	void checkPaths() {
		isTrue(::core::Path::exists("."));
		isTrue(::core::Path::exists("tests"));
		isTrue(::core::Path::exists("tests/filesystem_tests.hpp"));
		isFalse(::core::Path::exists("aaaaaaaaaaaaaaaaaaaaaaaa"));

		::core::Path path("tests");
		isTrue(path.exists());
		::core::Path bad_path("aaaaaaaaaaaaaaaaaa");
		isFalse(bad_path.exists());
	}

	void runTests() {
		std::cout << "Detected home directory: " << ::core::Path::home() << std::endl;
		std::cout << "Application data directory: " << ::core::Path::data() << std::endl;

		strangePaths();
		checkPaths();
	}
}}