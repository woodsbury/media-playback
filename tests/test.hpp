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

#ifndef _TEST_HPP
#define _TEST_HPP

#include <iostream>
#include <debug.hpp>

extern "C" {
#include <sys/resource.h>
}

namespace test {
	void pass();
	void fail();

	template<typename Tx, typename Ty>
	bool equal(Tx x, Ty y) {
		if (x == y) {
			pass();
			return true;
		} else {
			std::cout << "Equal test failed" << std::endl;
			std::cout << x << " is not equal to " << y << std::endl;
			fail();
			return false;
		}
	}

	template<typename Tx, typename Ty>
	bool notEqual(Tx x, Ty y) {
		if (x != y) {
			pass();
			return true;
		} else {
			std::cout << "Not equal test failed" << std::endl;
			std::cout << x << " is equal to " << y << std::endl;
			fail();
			return false;
		}
	}

	bool equal(double x, double y, double delta);
	bool notEqual(double x, double y, double delta);

	bool isTrue(bool x);
	bool isFalse(bool x);

/*
	Times how long the function passed to it takes to complete
*/
	void time(void (* function)(), int N);
}

#endif
