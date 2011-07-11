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

#include <debug.hpp>
#include <core/database.hpp>

extern "C" {
#include <sqlite3.h>
}

namespace sqlite {
/*
	Inheriting from this class will automatically initialise and shut down SQLite
*/
	class Initialiser {
		static unsigned int counter_;

	public:
		Initialiser() {
			if (counter_ == 0) {
				// SQLite is uninitialised
				dprint("Initialising SQLite");
				sqlite3_initialize();
			}

			++counter_;
		}

		~Initialiser() {
			--counter_;

			if (counter_ == 0) {
				// Last SQLite class destroyed
				dprint("Shutting down SQLite");
				sqlite3_shutdown();
			}
		}
	};

	unsigned int Initialiser::counter_(0);
}

namespace core {
// Database connection
	class DatabasePrivate
		: sqlite::Initialiser {
		sqlite3 * db_;
		bool opened_;

	public:
		DatabasePrivate(char const * location, int flags) {
			opened_ = sqlite3_open_v2(location, &db_, flags, NULL) == SQLITE_OK;
		}

		~DatabasePrivate() {
			sqlite3_close(db_);
		}

		bool opened() const {
			return opened_;
		}

		sqlite3 * connection() {
			return db_;
		}
	};

	Database::Database(std::string location, OpenMode mode) {
		int flags = 0;

		switch (mode) {
			case OpenMode::ReadOnly:
				flags = SQLITE_OPEN_READONLY;
				break;
			case OpenMode::ReadWrite:
				flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
				break;
		};

		p = new DatabasePrivate(location.c_str(), flags);
	}

	Database::~Database() {
		delete p;
	}

	bool Database::opened() const {
		return p->opened();
	}

// Prepared statement
	class StatementPrivate {
		sqlite3_stmt * stmt_;
		bool valid_;

	public:
		StatementPrivate(Database & db, char const * statement) {
			valid_ = sqlite3_prepare_v2(db.p->connection(), statement, -1, &stmt_, NULL) == SQLITE_OK;
		}

		~StatementPrivate() {
			sqlite3_finalize(stmt_);
		}

		bool valid() const {
			return valid_;
		}
	};

	Statement::Statement(Database & db, std::string statement)
		: p(new StatementPrivate(db, statement.c_str())) {}

	Statement::~Statement() {
		delete p;
	}

	bool Statement::valid() const {
		return p->valid();
	}
}
