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

#include <core/database.hpp>

namespace test { namespace database {
/*
	Test opening a database with different open modes
*/
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

/*
	Test trying to execute an invalid statement
*/
	void invalidStmt() {
		::core::Database db;
		isTrue(db.opened());
		::core::Statement stmt(db, "INVALID STATEMENT");
		isFalse(stmt.valid());
		isFalse(stmt.execute());
		isFalse(stmt.hasData());
	}

/*
	Test destroying a database before an attached statement is destroyed
*/
	void destroyDbBeforeStmt() {
		::core::Database * db = new ::core::Database;
		isTrue(db->opened());
		::core::Statement * stmt = new ::core::Statement(*db, "SELECT 1");
		isTrue(stmt->valid());

		delete db;
		isFalse(stmt->valid());
		delete stmt;
	}

/*
	Test retrieving data
*/
	void stmtHasData() {
		::core::Database db;
		isTrue(db.opened());

		::core::Statement stmt(db, "SELECT 1");
		isTrue(stmt.valid());
		isFalse(stmt.hasData());
		isTrue(stmt.execute());
		isTrue(stmt.hasData());
		stmt.reset();
		isFalse(stmt.hasData());

		isTrue(stmt.execute());
		equal(stmt.columns(), 1u);
		equalN(stmt.dataType(0u), ::core::Statement::Type::Integer);
		equal(stmt.toBinary(0u)[0], '1');
		equal(stmt.toInteger(0u), 1LL);
		equal(stmt.toReal(0u), 1.0);
		equal(stmt.toString(0u), "1");
		equal(stmt.toInteger(3u), 0LL);
		isFalse(stmt.nextRow());
		isFalse(stmt.hasData());
		equal(stmt.toInteger(0u), 0LL);
	}

/*
	Test inserting data into a database
*/
	void insertData() {
		::core::Database db;
		::core::Statement create(db, "CREATE TABLE test (col1 PRIMARY KEY, col2)");
		isTrue(create.valid());
		isTrue(create.execute());

		::core::Statement insert(db, "INSERT INTO test (col1, col2) VALUES (1, 'a')");
		isTrue(insert.valid());
		isTrue(insert.execute());

		::core::Statement insert2(db, "INSERT INTO test (col1, col2) VALUES (2, 'bb')");
		isTrue(insert2.valid());
		isTrue(insert2.execute());

		::core::Statement select(db, "SELECT * FROM test");
		isTrue(select.valid());
		isTrue(select.execute());
		isTrue(select.hasData());

		equal(select.columns(), 2u);
		equal(select.toInteger(0u), 1);
		equal(select.toString(1u), "a");

		isTrue(select.nextRow());
		isTrue(select.hasData());
		equal(select.toInteger(0u), 2);
		equal(select.toString(1u), "bb");

		isFalse(select.nextRow());
		isFalse(select.hasData());
	}

	void runTests() {
		openDb();
		invalidStmt();
		destroyDbBeforeStmt();
		stmtHasData();
		insertData();
	}
}}
