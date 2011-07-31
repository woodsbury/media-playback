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
#include <core/filesystem.hpp>
#include <toolkit/library.hpp>

namespace toolkit {
	MediaItem::MediaItem(std::string title, std::string uri)
		: title_(title), uri_(uri) {}

	MediaItem::MediaItem(MediaItem const & media_item)
		: title_(media_item.title_), uri_(media_item.uri_) {}

	MediaItem::MediaItem(MediaItem && media_item) {
		swap(title_, media_item.title_);
		swap(uri_, media_item.uri_);
	}

/*
	Returns the title of the media item
*/
	std::string MediaItem::title() const {
		return title_;
	}

/*
	Returns the URI pointing to the location of the media item
*/
	std::string MediaItem::uri() const {
		return uri_;
	}

	Library::Library()
		: core::Database(core::Path::data() + "/library.db") {
		if (opened() && (tables().size() == 0u)) {
			// Library database hasn't been initialised yet
			dprint("Creating library");
			core::Statement create_item_table(*this,
					"CREATE TABLE items (key INTEGER PRIMARY KEY, name TEXT, uri TEXT, type TEXT)");
			assert(create_item_table.execute());
		}
	}

	std::vector< MediaItem > Library::list(Library::Type type) {
		std::vector< MediaItem > items;

		return items;
	}
}
