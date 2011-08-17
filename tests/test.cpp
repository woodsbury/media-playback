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
#include <iostream>
#include <debug.hpp>

extern "C" {
#include <sys/resource.h>
}

namespace test {
	unsigned int tests_passed;
	unsigned int tests_total;

	/*
		A test has passed
	*/
	void pass() {
		++tests_passed;
		++tests_total;
	}

	/*
		A test has failed
	*/
	void fail() {
		++tests_total;
	}

	/*
		Test if the arguments are equal
	*/
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
	bool equalN(Tx x, Ty y) {
		if (x == y) {
			pass();
			return true;
		} else {
			std::cout << "Equal test failed" << std::endl;
			fail();
			return false;
		}
	}

	/*
		Test if the arguments are not equal
	*/
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
	template<typename Tx, typename Ty>
	bool notEqualN(Tx x, Ty y) {
		if (x != y) {
			pass();
			return true;
		} else {
			std::cout << "Not equal test failed" << std::endl;
			fail();
			return false;
		}
	}

	/*
		Test if the arguments are within an acceptable delta
	*/
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

	/*
		Test if the arguments are outside an acceptable delta
	*/
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

	/*
		Test if the expression is true
	*/
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

	/*
		Test if the expression is false
	*/
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

	/*
		Time how long a function takes to return, averaging over N times
	*/
	void time(void (* function)(), int N) {
		// Disable debug output to prevent filling the screen
		dprint_enable(false);

		rusage start_usage;
		rusage loop_usage;
		rusage end_usage;

		// Starting time
		getrusage(RUSAGE_SELF, &start_usage);

		// Find the overhead of running the loop
		for (int i = 0; i < N; ++i) {}
		getrusage(RUSAGE_SELF, &loop_usage);

		// Find the time to run the function
		for (int i = 0; i < N; ++i) {
			function();
		}
		getrusage(RUSAGE_SELF, &end_usage);

		// Convert to a double representation
		double user_loop_time = (loop_usage.ru_utime.tv_sec - start_usage.ru_utime.tv_sec)
		                        + (loop_usage.ru_utime.tv_usec - start_usage.ru_utime.tv_usec) / 1000000.0;
		double user_spent_time = (end_usage.ru_utime.tv_sec - loop_usage.ru_utime.tv_sec)
		                         + (end_usage.ru_utime.tv_usec - loop_usage.ru_utime.tv_usec) / 1000000.0 - user_loop_time;
		double system_loop_time = (loop_usage.ru_stime.tv_sec - start_usage.ru_stime.tv_sec)
		                          + (loop_usage.ru_stime.tv_usec - start_usage.ru_stime.tv_usec) / 1000000.0;
		double system_spent_time = (end_usage.ru_stime.tv_sec - loop_usage.ru_stime.tv_sec)
		                           + (end_usage.ru_stime.tv_usec - loop_usage.ru_stime.tv_usec) / 1000000.0 - system_loop_time;

		// Normalise
		user_spent_time /= N;
		system_spent_time /= N;

		std::cout << "User time spent: ";
		if (user_spent_time < 1e-3) {
			std::cout << user_spent_time * 1e6 << " microsecond(s)" << std::endl;
		} else if (user_spent_time < 1.0) {
			std::cout << user_spent_time * 1e3 << " millisecond(s)" << std::endl;
		} else {
			std::cout << user_spent_time << " second(s)" << std::endl;
		}

		std::cout << "System time spent: ";
		if (system_spent_time < 1e-3) {
			std::cout << system_spent_time * 1e6 << " microsecond(s)" << std::endl;
		} else if (system_spent_time < 1.0) {
			std::cout << system_spent_time * 1e3 << " millisecond(s)" << std::endl;
		} else {
			std::cout << system_spent_time << " second(s)" << std::endl;
		}

		// Reenable debug output
		dprint_enable(true);
	}
}

namespace {
	void printResults() {
		std::cout << test::tests_passed << "/" << test::tests_total << " passed" << std::endl;
		test::tests_passed = 0;
		test::tests_total = 0;
	}
}

#include "database_tests.hpp"
#include "filesystem_tests.hpp"
#include "inspector_tests.hpp"

int main(int, char **) {
	std::cout << "Programme Name: " << NAME << std::endl;
	std::cout << "Programme Version: " << VERSION << std::endl;

	std::cout << "\nRunning database tests" << std::endl;
	test::database::runTests();
	printResults();

	std::cout << "\nRunning filesystem tests" << std::endl;
	test::filesystem::runTests();
	printResults();

	std::cout << "\nRunning inspector tests" << std::endl;
	test::inspector::runTests();
	printResults();

	return 0;
}
