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
	class MediaItem {
		std::string title_;
		std::string uri_;
		std::string thumbnail_;

	public:
		MediaItem(std::string title, std::string uri, std::string thumbnail_file = std::string());
		MediaItem(MediaItem const & media_item);
		MediaItem(MediaItem && media_item);

		std::string title() const;
		std::string uri() const;
		std::string thumbnail_file() const;
	};

	class Library
		: private core::Database {
		core::Statement * count_stmt_;
		core::Statement * list_stmt_;

	public:
		enum class Type {
			All,
			Music,
			Movies
		};

		Library();
		~Library();

		unsigned long long count(Type type);
		std::vector< MediaItem > list(Type type, unsigned long long size = 0);
		std::vector< MediaItem > listNext(unsigned long long size = 0);
	};
}

#endif
