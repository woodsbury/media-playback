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
	MediaItem::MediaItem(std::string title, std::string uri, std::string thumbnail_file)
		: title_(title), uri_(uri), thumbnail_(thumbnail_file) {}

	MediaItem::MediaItem(MediaItem const & media_item)
		: title_(media_item.title_), uri_(media_item.uri_), thumbnail_(media_item.thumbnail_file()) {}

	MediaItem::MediaItem(MediaItem && media_item) {
		swap(title_, media_item.title_);
		swap(uri_, media_item.uri_);
		swap(thumbnail_, media_item.thumbnail_);
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

/*
	Returns the file with a thumbnail of the media item
*/
	std::string MediaItem::thumbnail_file() const {
		return thumbnail_;
	}

	Library::Library()
		: core::Database(core::Path::data() + "/library.db"), list_stmt_(nullptr) {
		if (opened() && (tables().size() == 0u)) {
			// Library database hasn't been initialised yet
			dprint("Creating library");
			core::Statement create_item_table(*this,
					"CREATE TABLE items "
					"(key INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, uri TEXT, thumbnail TEXT, type TEXT)");
			assert(create_item_table.valid());
			create_item_table.execute();
		}
	}

	Library::~Library() {
		delete list_stmt_;
	}

	std::vector< MediaItem > Library::list(Library::Type type) {
		if (list_stmt_ == nullptr) {
			list_stmt_ = new core::Statement(*this, "SELECT name, uri, thumbnail FROM items WHERE type LIKE ?");
		}

		switch (type) {
		case Type::All:
			list_stmt_->bind(1u, "%");
			break;
		case Type::Music:
			list_stmt_->bind(1u, "music");
			break;
		case Type::Movies:
			list_stmt_->bind(1u, "movie");
			break;
		}

		assert(list_stmt_->valid());
		list_stmt_->execute();

		if (list_stmt_->hasData()) {
			std::vector< MediaItem > items;

			do {
				if (list_stmt_->dataType(2u) == core::Statement::Type::Null) {
					items.emplace_back(list_stmt_->toText(0u), list_stmt_->toText(1u));
				} else {
					items.emplace_back(list_stmt_->toText(0u), list_stmt_->toText(1u), list_stmt_->toText(2u));
				}
			} while (list_stmt_->nextRow());

			return items;
		}

		dprint("Library empty");
		return std::vector< MediaItem >();
	}
}
