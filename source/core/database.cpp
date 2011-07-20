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

namespace { namespace sqlite {
/*
	Inheriting from this class will automatically initialise and shut down SQLite
*/
	class Initialiser {
		class private_initialiser {
		public:
			private_initialiser() {
				dprint("Initialising SQLite");
				sqlite3_initialize();
			}

			~private_initialiser() {
				dprint("Shutting down SQLite");
				sqlite3_shutdown();
			}
		};

	public:
		Initialiser() {
			static private_initialiser initialiser;
		}
	};
}}

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

		inline bool opened() const;

		inline sqlite3 * connection();

		inline void addStatement(StatementPrivate * const statement);

		inline void removeStatement(StatementPrivate * const statement);
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

		inline bool valid() const;

		inline bool execute();
		inline bool hasData() const;
		inline void reset();

		inline unsigned int columns() const;

		inline Statement::Type dataType(unsigned int column) const;
		inline std::vector< unsigned char > toBinary(unsigned int column) const;
		inline long long toInteger(unsigned int column) const;
		inline double toReal(unsigned int column) const;
		inline char const * toText(unsigned int column) const;

		inline bool bind(unsigned int index) const;
		inline bool bind(unsigned int index, unsigned char const * binary, unsigned long long size) const;
		inline bool bind(unsigned int index, long long integer) const;
		inline bool bind(unsigned int index, double real) const;
		inline bool bind(unsigned int index, char const * text) const;
	};

// Database connection
	DatabasePrivate::DatabasePrivate(char const * location, int flags) {
		dprint("Opening %s", location);
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

// Public class
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

		p = new DatabasePrivate(location.empty() ? ":memory:" : location.c_str(), flags);
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
					has_data_ = false;
					--loop_count;
					usleep(3);
					break;
				case SQLITE_DONE:
					// The statement executed but didn't return any rows
					has_data_ = false;
					return true;
				case SQLITE_ROW:
					// The statement executed and has row database
					has_data_ = true;
					return true;
				default:
					// An error of some kind occurred
					has_data_ = false;
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

/*
	Returns the number of columns of any current result set
*/
	unsigned int StatementPrivate::columns() const {
		return sqlite3_data_count(stmt_);
	}

/*
	Return the data type of a column
*/
	Statement::Type StatementPrivate::dataType(unsigned int column) const {
		switch (sqlite3_column_type(stmt_, column)) {
		case SQLITE_INTEGER:
			return Statement::Type::Integer;
		case SQLITE_FLOAT:
			return Statement::Type::Real;
		case SQLITE_BLOB:
			return Statement::Type::Binary;
		case SQLITE_NULL:
			return Statement::Type::Null;
		case SQLITE_TEXT:
			return Statement::Type::Text;
		default:
			return Statement::Type::Null;
		};
	}

/*
	Return the value as a binary array
*/
	std::vector< unsigned char > StatementPrivate::toBinary(unsigned int column) const {
		unsigned char const * data = static_cast< unsigned char const * >(sqlite3_column_blob(stmt_, column));
		int size = sqlite3_column_bytes(stmt_, column);
		assert(size > 0);
		return std::vector< unsigned char >(data, data + size);
	}

/*
	Return the value in column as an integer
*/
	long long StatementPrivate::toInteger(unsigned int column) const {
		return column < columns() ? sqlite3_column_int64(stmt_, column) : 0LL;
	}

/*
	Return the value in column as floating point
*/
	double StatementPrivate::toReal(unsigned int column) const {
		return column < columns() ? sqlite3_column_double(stmt_, column) : 0.0;
	}

/*
	Return the value in column as a string
*/
	char const * StatementPrivate::toText(unsigned int column) const {
		return column < columns() ? reinterpret_cast< char const * >(sqlite3_column_text(stmt_, column)) : "";
	}

/*
	Binds a null value to a parameter
*/
	bool StatementPrivate::bind(unsigned int index) const {
		return sqlite3_bind_null(stmt_, index) == SQLITE_OK;
	}

/*
	Binds a binary array to a parameter
*/
	bool StatementPrivate::bind(unsigned int index, unsigned char const * binary, unsigned long long size) const {
		return sqlite3_bind_blob(stmt_, index, static_cast< void const * >(binary), size,
				SQLITE_TRANSIENT) == SQLITE_OK;
	}

/*
	Binds an integer to a parameter
*/
	bool StatementPrivate::bind(unsigned int index, long long integer) const {
		return sqlite3_bind_int64(stmt_, index, integer) == SQLITE_OK;
	}

/*
	Binds a floating point to a parameter
*/
	bool StatementPrivate::bind(unsigned int index, double real) const {
		return sqlite3_bind_double(stmt_, index, real) == SQLITE_OK;
	}

/*
	Binds a string to a parameter
*/
	bool StatementPrivate::bind(unsigned int index, char const * text) const {
		return sqlite3_bind_text(stmt_, index, text, -1, SQLITE_TRANSIENT) == SQLITE_OK;
	}

// Public class
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

	bool Statement::nextRow() const {
		return p->execute() && p->hasData();
	}

	unsigned int Statement::columns() const {
		return p->columns();
	}

	Statement::Type Statement::dataType(unsigned int column) const {
		return p->dataType(column);
	}

	std::vector< unsigned char > Statement::toBinary(unsigned int column) const {
		return p->toBinary(column);
	}

	long long Statement::toInteger(unsigned int column) const {
		return p->toInteger(column);
	}

	std::string Statement::toText(unsigned int column) const {
		return p->toText(column);
	}

	double Statement::toReal(unsigned int column) const {
		return p->toReal(column);
	}

	bool Statement::bind(unsigned int index) const {
		p->reset();
		return p->bind(index);
	}

	bool Statement::bind(unsigned int index, std::vector< unsigned char > binary) const {
		p->reset();
		return p->bind(index, &(binary[0]), binary.size());
	}

	bool Statement::bind(unsigned int index, long long integer) const {
		p->reset();
		return p->bind(index, integer);
	}

	bool Statement::bind(unsigned int index, double real) const {
		p->reset();
		return p->bind(index, real);
	}

	bool Statement::bind(unsigned int index, std::string text) const {
		p->reset();
		return p->bind(index, text.c_str());
	}
}
