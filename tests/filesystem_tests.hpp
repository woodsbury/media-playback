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
	void createPaths() {
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

		relative_path.set("./new/path/");
		equal(relative_path.toString(), "new/path");
		isFalse(relative_path.absolute());

		::core::Path path_start_dots("../starts/with/relative");
		equal(path_start_dots.toString(), "../starts/with/relative");
		isFalse(path_start_dots.absolute());

		::core::Path multiple_dots("./../starts/./with/../two/dots");
		equal(multiple_dots.toString(), "../starts/two/dots");
		isFalse(multiple_dots.absolute());
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

/*
	Test for creating absolute directories
*/
	void absolutePaths() {
		::core::Path relative_current(".");
		isTrue(relative_current.makeAbsolute());
		equal(relative_current.toString(), ::core::Path::current());

		::core::Path relative_parent("..");
		isTrue(relative_parent.makeAbsolute());
		equal(relative_parent.toString(), ::core::Path::current().substr(0, relative_parent.toString().length()));
	}

/*
	Test for creating directories from paths
*/
	void makeDir() {
		::core::Path path("tests/aaaaaaaaaaaaa");
		isFalse(path.exists());
		isTrue(path.create());
		isTrue(path.exists());
		isTrue(::core::Path::remove(path.toString()));
		isFalse(path.exists());
	}

	void timeFs() {
		::core::Path path("..");
		path.makeAbsolute();
		path.toString();
		path.exists();
		path.set("/new/long/path/with/../various/////./extra/parts/.././../");
		path.toString();
		path.exists();
		path.makeAbsolute();
		path.set();
		path.toString();
		path.exists();
		path.makeAbsolute();
	}

	void runTests() {
		std::cout << "Detected home directory: " << ::core::Path::home() << std::endl;
		std::cout << "Application data directory: " << ::core::Path::data() << std::endl;
		std::cout << "Current working directory: " << ::core::Path::current() << std::endl;

		createPaths();
		checkPaths();
		absolutePaths();
		makeDir();

		time(timeFs, 1000);
	}
}}
