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

#include <cmath>

#include "test.hpp"

namespace test {
	unsigned int tests_passed;
	unsigned int tests_total;

	void pass() {
		++tests_passed;
		++tests_total;
	}

	void fail() {
		++tests_total;
	}

	bool equal(double x, double y, double delta) {
		if (std::abs(x - y) < delta) {
			pass();
			return true;
		} else {
			std::cout << "Equal test failed" << std::endl;
			std::cout << x << " is not within " << delta << " of " << y << std::endl;
			fail();
			return false;
		}
	}

	bool notEqual(double x, double y, double delta) {
		if (std::abs(x - y) > delta) {
			pass();
			return true;
		} else {
			std::cout << "Not equal test failed" << std::endl;
			std::cout << x << " is within " << delta << " of " << y << std::endl;
			fail();
			return false;
		}
	}

	bool isTrue(bool x) {
		if (x) {
			pass();
			return true;
		} else {
			std::cout << "True test failed" << std::endl;
			fail();
			return false;
		}
	}

	bool isFalse(bool x) {
		if (x) {
			std::cout << "False test failed" << std::endl;
			fail();
			return false;
		} else {
			pass();
			return true;
		}
	}
}

namespace {
	void printResults() {
		std::cout << test::tests_passed << "/" << test::tests_total << " passed" << std::endl;
		test::tests_passed = 0;
		test::tests_total = 0;
	}
}

#include "mediatests.hpp"

int main(int, char **) {
	std::cout << "Programme Name: " << NAME << std::endl;
	std::cout << "Programme Version: " << VERSION << std::endl;

	std::cout << "\nRunning media tests" << std::endl;
	test::media::runTests();
	printResults();

	return 0;
}
