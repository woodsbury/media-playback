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

extern "C" {
#include <sqlite.h>
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

	int Initialiser::counter_(0);
}

namespace core {
	class DatabasePrivate
		: sqlite::Initialiser {
		sqlite3 * db_;

	public:
		DatabasePrivate(char const * location, int flags) {
			sqlite3_open_v2(location, &db_, flags, NULL);
		}

		~DatabasePrivate() {
			sqlite3_close(db_);
		}
	};

	Database::Database(std::string location, OpenMode mode) {
		int flags;

		switch (mode) {
			case OpenMode::ReadOnly:
				flags = SQLITE_OPEN_READONLY;
				break;
			case OpenMode::ReadWrite:`
				flags = SQLITE_OPEN_READWRITE;
				break;
		};

		p = new DatabasePrivate(location.c_str(), flags);
	}

	Database::~Database() {
		delete p;
	}
}
