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

#include <set>
#include <debug.hpp>
#include <core/database.hpp>

extern "C" {
#include <sqlite3.h>
#include <unistd.h>
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
// Private class declarations
	class DatabasePrivate
		: sqlite::Initialiser {
		sqlite3 * db_;
		bool opened_;

		std::set< StatementPrivate * > statements_;

	public:
		DatabasePrivate(char const * location, int flags);
		~DatabasePrivate();

		bool opened() const;

		sqlite3 * connection();

		void addStatement(StatementPrivate * const statement);

		void removeStatement(StatementPrivate * const statement);
	};

	class StatementPrivate
		: sqlite::Initialiser {
		friend class DatabasePrivate;

		sqlite3_stmt * stmt_;
		bool valid_;
		bool has_data_;

		DatabasePrivate * const db_;

	public:
		StatementPrivate(Database & db, char const * statement);
		~StatementPrivate();

		bool valid() const;

		bool execute();
		bool hasData() const;
		void reset();
	};

// Database connection
	DatabasePrivate::DatabasePrivate(char const * location, int flags) {
		opened_ = sqlite3_open_v2(location, &db_, flags, NULL) == SQLITE_OK;
	}

	DatabasePrivate::~DatabasePrivate() {
		// Finalise all statements that still exist
		for (std::set< StatementPrivate * >::iterator i = statements_.begin(); i != statements_.end(); ++i) {
			(*i)->valid_ = false;
			sqlite3_finalize((*i)->stmt_);
		}

		sqlite3_close(db_);
	}

/*
	Indicates whether the database was opened successfully
*/
	bool DatabasePrivate::opened() const {
		return opened_;
	}

/*
	Returns the SQLite connection
*/
	sqlite3 * DatabasePrivate::connection() {
		return db_;
	}

/*
	Called when a new statement has been created on this connection
*/
	void DatabasePrivate::addStatement(StatementPrivate * const statement) {
		statements_.insert(statement);
	}

/*
	Called when a statement on this connection is destroyed
*/
	void DatabasePrivate::removeStatement(StatementPrivate * const statement) {
		statements_.erase(statement);
	}

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
	StatementPrivate::StatementPrivate(Database & db, char const * statement)
		: has_data_(false), db_(db.p) {
		valid_ = sqlite3_prepare_v2(db_->connection(), statement, -1, &stmt_, NULL) == SQLITE_OK;

		if (valid_) {
			db_->addStatement(this);
		}
	}

	StatementPrivate::~StatementPrivate() {
		if (valid_) {
			db_->removeStatement(this);
			sqlite3_finalize(stmt_);
		}
	}

/*
	Indicates whether the statement was prepared successfully
*/
	bool StatementPrivate::valid() const {
		return valid_;
	}

/*
	Executes the prepared statement
*/
	bool StatementPrivate::execute() {
		if (valid_) {
			int loop_count = 5;

			do {
				switch (sqlite3_step(stmt_)) {
					case SQLITE_BUSY:
						// Couldn't get lock on database, wait and try again
						--loop_count;
						usleep(3);
						break;
					case SQLITE_DONE:
						// The statement executed but didn't return any rows
						return true;
					case SQLITE_ROW:
						// The statement executed and has row database
						has_data_ = true;
						return true;
					default:
						// An error of some kind occurred
						return false;
				}
			} while (loop_count > 0);
		}

		return false;
	}

/*
	Indicates whether the statement has data available
*/
	bool StatementPrivate::hasData() const {
		return has_data_;
	}

/*
	Resets the statement ignoring any remaining rows that could be returned
*/
	void StatementPrivate::reset() {
		has_data_ = false;
		sqlite3_reset(stmt_);
	}

	Statement::Statement(Database & db, std::string statement)
		: p(new StatementPrivate(db, statement.c_str())) {}

	Statement::~Statement() {
		delete p;
	}

	bool Statement::valid() const {
		return p->valid();
	}

	bool Statement::execute() const {
		return p->execute();
	}

	bool Statement::hasData() const {
		return p->hasData();
	}

	void Statement::reset() const {
		p->reset();
	}
}
