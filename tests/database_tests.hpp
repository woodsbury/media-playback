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
		equal(stmt.toBinary(0u).size(), 1u);
		equal(stmt.toInteger(0u), 1LL);
		equal(stmt.toReal(0u), 1.0);
		equal(stmt.toText(0u), "1");
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
		equal(select.toText(1u), "a");

		isTrue(select.nextRow());
		isTrue(select.hasData());
		equal(select.toInteger(0u), 2);
		equal(select.toText(1u), "bb");

		isFalse(select.nextRow());
		isFalse(select.hasData());
	}

/*
	Test for binding values
*/
	void bindValues() {
		::core::Database db;
		::core::Statement stmt(db, "SELECT ?");
		isTrue(stmt.valid());

		isTrue(stmt.execute());
		isTrue(stmt.hasData());
		equal(stmt.columns(), 1u);
		equalN(stmt.dataType(0u), ::core::Statement::Type::Null);

		stmt.reset();
		isTrue(stmt.bind(1u, std::vector< unsigned char >(1, 'a')));
		isTrue(stmt.execute());
		isTrue(stmt.hasData());
		equal(stmt.columns(), 1u);
		equalN(stmt.dataType(0u), ::core::Statement::Type::Binary);
		equal(stmt.toBinary(0u)[0], 'a');

		stmt.reset();
		isTrue(stmt.bind(1u, 3LL));
		isTrue(stmt.execute());
		isTrue(stmt.hasData());
		equal(stmt.columns(), 1u);
		equalN(stmt.dataType(0u), ::core::Statement::Type::Integer);
		equal(stmt.toInteger(0u), 3LL);

		stmt.reset();
		isTrue(stmt.bind(1u, 3.2));
		isTrue(stmt.execute());
		isTrue(stmt.hasData());
		equal(stmt.columns(), 1u);
		equalN(stmt.dataType(0u), ::core::Statement::Type::Real);
		equal(stmt.toReal(0u), 3.2, 0.0001);

		stmt.reset();
		isTrue(stmt.bind(1u, "abc"));
		isTrue(stmt.execute());
		isTrue(stmt.hasData());
		equal(stmt.columns(), 1u);
		equalN(stmt.dataType(0u), ::core::Statement::Type::Text);
		equal(stmt.toText(0u), "abc");
	}

/*
	Test for checking database tables
*/
	void checkTables() {
		::core::Database db;
		::core::Statement stmt(db, "CREATE TABLE test (col1 PRIMARY KEY)");
		isTrue(stmt.execute());
		std::vector< std::string > tables = db.tables();
		equal(tables.size(), 1u);
		equal(tables.at(0), "test");

		::core::Statement stmt2(db, "CREATE TABLE test2 (col1 PRIMARY KEY)");
		isTrue(stmt2.execute());
		tables = db.tables();
		equal(tables.size(), 2u);
		isTrue((tables.at(0) == "test") || (tables.at(0) == "test2"));
		isTrue((tables.at(1) == "test") || (tables.at(1) == "test2"));
		notEqual(tables.at(0), tables.at(1));

		::core::Statement stmt3(db, "DROP TABLE test");
		isTrue(stmt3.execute());
		tables = db.tables();
		equal(tables.size(), 1u);
		equal(tables.at(0), "test2");
	}

	void timeDb() {
		::core::Database db;
		if (!db.opened()) {
			std::cout << "Invalid database in timing test" << std::endl;
			return;
		}
		::core::Statement stmt(db, "CREATE TABLE test (col1 PRIMARY KEY, col2)");
		if (!stmt.valid()) {
			std::cout << "Invalid statement in timing test" << std::endl;
			return;
		}
		stmt.execute();
		::core::Statement stmt2(db, "INSERT INTO test (col1, col2) VALUES (1, 1)");
		if (!stmt2.valid()) {
			std::cout << "Invalid statement in timing test" << std::endl;
			return;
		}
		stmt2.execute();
		::core::Statement stmt3(db, "INSERT INTO test (col1, col2) VALUES (2, 2)");
		if (!stmt3.valid()) {
			std::cout << "Invalid statement in timing test" << std::endl;
			return;
		}
		stmt3.execute();
		::core::Statement stmt4(db, "SELECT * FROM test");
		if (!stmt4.valid()) {
			std::cout << "Invalid statement in timing test" << std::endl;
			return;
		}
		stmt4.execute();

		long long value;
		for (unsigned int i = 0; i < 2; ++i) {
			value = stmt4.toInteger(1u);
		}
		value = value;

		::core::Statement stmt5(db, "DELETE FROM test");
		if (!stmt5.valid()) {
			std::cout << "Invalid statement in timing test" << std::endl;
			return;
		}
		stmt5.execute();
	}

	void runTests() {
		openDb();
		invalidStmt();
		destroyDbBeforeStmt();
		stmtHasData();
		insertData();
		bindValues();
		checkTables();

		time(timeDb, 50);
	}
}}
