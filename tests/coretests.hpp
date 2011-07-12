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

#include <core.hpp>

namespace test { namespace core {
// Database tests
	void openDb() {
		{
			::core::Database db("./tests/test.db");
			isTrue(db.opened());
		}

		::core::Database db2("./tests/test.db", ::core::Database::OpenMode::ReadOnly);
		isTrue(db2.opened());

		::core::Database db3("aaaaaaaaaaaaaaaaaaaaa", ::core::Database::OpenMode::ReadOnly);
		isFalse(db3.opened());

		equal(std::remove("./tests/test.db"), 0);
	}

	void invalidStmt() {
		{
			::core::Database db("./tests/test.db");
			isTrue(db.opened());
			::core::Statement stmt(db, "INVALID STATEMENT");
			isFalse(stmt.valid());
		}

		equal(std::remove("./tests/test.db"), 0);
	}

	void destroyDbBeforeStmt() {
		::core::Database * db = new ::core::Database("./tests/test.db");
		isTrue(db->opened());
		::core::Statement * stmt = new ::core::Statement(*db, "SELECT 1");
		isTrue(stmt->valid());

		delete db;
		isFalse(stmt->valid());
		delete stmt;

		equal(std::remove("./tests/test.db"), 0);
	}

	void timeConnectDb() {
		::core::Database db("./tests/test.db");
	}

	void runTests() {
		// Database tests
		openDb();
		invalidStmt();
		destroyDbBeforeStmt();

		std::cout << "Timing connecting to a database" << std::endl;
		time(&timeConnectDb, 25);
		std::remove("./tests/test.db");
	}
}}
