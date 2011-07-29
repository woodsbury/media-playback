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
#include <vector>
#include <core/noncopiable.hpp>

namespace core {
	class DatabasePrivate;
	class StatementPrivate;

/*
	Creates a connection to a database
*/
	class Database
		: NonCopiable {
		friend class StatementPrivate;

		DatabasePrivate * p;

	public:
		enum class OpenMode {
			ReadOnly,
			ReadWrite
		};

		Database(std::string location = std::string(), OpenMode mode = OpenMode::ReadWrite);
		~Database();

		bool opened() const;

		std::vector< std::string > tables();
	};

/*
	Represents a statement to execute on a database connection
*/
	class Statement
		: NonCopiable {
		StatementPrivate * p;

	public:
		enum class Type {
			Null,
			Binary,
			Integer,
			Real,
			Text
		};

		Statement(Database & db, std::string statement);
		~Statement();

		bool valid() const;

		bool execute() const;
		bool hasData() const;
		void reset() const;

		unsigned int columns() const;

		Type dataType(unsigned int column) const;
		std::vector< unsigned char > toBinary(unsigned int column) const;
		long long toInteger(unsigned int column) const;
		double toReal(unsigned int column) const;
		std::string toText(unsigned int column) const;

		bool nextRow() const;

		bool bind(unsigned int index) const;
		bool bind(unsigned int index, std::vector< unsigned char > binary) const;
		bool bind(unsigned int index, long long integer) const;
		bool bind(unsigned int index, double real) const;
		bool bind(unsigned int index, std::string text) const;
	};
}

#endif
