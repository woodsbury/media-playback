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

namespace {
/*
	Binds the type to the specified index in the statement
*/
	void bind_type(core::Statement * stmt, toolkit::Library::Type type, unsigned int index) {
		assert(stmt != nullptr);

		switch (type) {
		case toolkit::Library::Type::All:
			stmt->bind(index, "%");
			break;
		case toolkit::Library::Type::Music:
			stmt->bind(index, "music");
			break;
		case toolkit::Library::Type::Movies:
			stmt->bind(index, "movie");
			break;
		}
	}

/*
	Fetches the next specified number of items
*/
	std::vector< toolkit::MediaItem > fetch(core::Statement * stmt, unsigned long long size) {
		if ((stmt == nullptr) || (!stmt->hasData())) {
			return std::vector< toolkit::MediaItem >();
		}

		std::vector< toolkit::MediaItem > items;
		unsigned long long i = 0;

		if (size == 0) {
			size = items.max_size();
		}

		do {
			if (stmt->dataType(2u) == core::Statement::Type::Null) {
				items.emplace_back(stmt->toText(0u), stmt->toText(1u));
			} else {
				items.emplace_back(stmt->toText(0u), stmt->toText(1u), stmt->toText(2u));
			}
			++i;
		} while (stmt->nextRow() && (i < size));

		return items;
	}
}

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
		: core::Database(core::Path::data() + "/library.db"), add_stmt_(nullptr), count_stmt_(nullptr),
		  list_stmt_(nullptr) {
		if (opened() && (tables().size() == 0u)) {
			// Library database hasn't been initialised yet
			dprint("Creating library");
			core::Statement create_item_table(*this,
					"CREATE TABLE items "
					"(key INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, "
					"uri TEXT NOT NULL, thumbnail TEXT, type TEXT)");
			assert(create_item_table.valid());
			create_item_table.execute();
		}
	}

	Library::~Library() {
		delete add_stmt_;
		delete count_stmt_;
		delete list_stmt_;
	}

/*
	Adds a new entry to the library
*/
	void Library::add(std::string title, std::string uri, Library::Type type, std::string thumbnail_file) {
		if (add_stmt_ == nullptr) {
			add_stmt_ = new core::Statement(*this,
					"INSERT INTO items (name, uri, type, thumbnail) VALUES (?, ?, ?, ?)");
		} else {
			add_stmt_->reset();
		}

		add_stmt_->bind(1u, title);
		add_stmt_->bind(2u, uri);
		bind_type(add_stmt_, type, 3u);

		if (thumbnail_file.empty()) {
			add_stmt_->bind(4u);
		} else {
			add_stmt_->bind(4u, thumbnail_file);
		}

		assert(add_stmt_->valid());
		add_stmt_->execute();
	}

/*
	Count the number of items in the media library of a given type
*/
	unsigned long long Library::count(Library::Type type) {
		if (count_stmt_ == nullptr) {
			count_stmt_ = new core::Statement(*this, "SELECT COUNT(key) FROM items WHERE type LIKE ?");
		} else {
			count_stmt_->reset();
		}

		bind_type(count_stmt_, type, 1u);

		assert(count_stmt_->valid());
		count_stmt_->execute();
		assert(count_stmt_->hasData());

		return count_stmt_->toInteger(0u);
	}

/*
	Return the specified number of items of the given type from the media library
*/
	std::vector< MediaItem > Library::list(Library::Type type, unsigned long long size) {
		if (list_stmt_ == nullptr) {
			list_stmt_ = new core::Statement(*this, "SELECT name, uri, thumbnail FROM items WHERE type LIKE ?");
		} else {
			list_stmt_->reset();
		}

		bind_type(list_stmt_, type, 1u);

		assert(list_stmt_->valid());
		list_stmt_->execute();

		return fetch(list_stmt_, size);
	}

/*
	Fetch the next specified number of items in the list
*/
	std::vector< MediaItem > Library::listNext(unsigned long long size) {
		return fetch(list_stmt_, size);
	}
}
