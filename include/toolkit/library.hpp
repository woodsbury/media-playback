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

#ifndef _TOOLKIT_LIBRARY_HPP
#define _TOOLKIT_LIBRARY_HPP

#include <string>
#include <vector>
#include <core/database.hpp>

namespace toolkit {
	class LibraryItem {
		long long id_;

		std::string title_;
		std::string uri_;
		std::string thumbnail_;

	public:
		LibraryItem(long long id, std::string title, std::string uri, std::string thumbnail_file = std::string());
		LibraryItem(LibraryItem const & library_item);
		LibraryItem(LibraryItem && library_item);

		long long id() const;

		std::string title() const;
		std::string uri() const;
		std::string thumbnailFile() const;
	};

	class Library
		: private core::Database {
	public:
		enum class Type {
			All,
			Music,
			Movies
		};

	private:
		core::Statement * add_stmt_;
		core::Statement * count_stmt_;
		core::Statement * list_stmt_;
		core::Statement * search_stmt_;

		core::Statement * type_stmt_;
		core::Statement * album_stmt_;

		void initialise_db();

		long long type_id(Type type);
		long long album_id(std::string album);

	public:
		Library();
		~Library();

		void add(std::string title, std::string uri, Type type, std::string thumbnail_file = std::string(), std::string album = std::string());

		unsigned long long count(Type type);
		std::vector< LibraryItem > list(Type type);
		std::vector< LibraryItem > search(Type type, std::string term);
	};
}

#endif
