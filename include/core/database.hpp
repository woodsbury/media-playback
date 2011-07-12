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

#ifndef _CORE_DATABASE_HPP
#define _CORE_DATABASE_HPP

#include <string>

namespace core {
	class DatabasePrivate;
	class StatementPrivate;

/*
	Creates a connection to a database
*/
	class Database {
		friend class StatementPrivate;

		DatabasePrivate * p;

	public:
		enum class OpenMode {
			ReadOnly,
			ReadWrite
		};

		Database(std::string location, OpenMode mode = OpenMode::ReadWrite);
		~Database();

		bool opened() const;
	};

/*
	Represents a statement to execute on a database connection
*/
	class Statement {
		StatementPrivate * p;

	public:
		Statement(Database & db, std::string statement);
		~Statement();

		bool valid() const;

		bool execute() const;
		bool hasData() const;
		void reset() const;
	};
}

#endif
