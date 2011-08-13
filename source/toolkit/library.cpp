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
	long long const db_version(1);

/*
	Fetches the next specified number of items
*/
	inline std::vector< toolkit::MediaItem > fetch(core::Statement * stmt) {
		if ((stmt == nullptr) || (!stmt->hasData())) {
			return std::vector< toolkit::MediaItem >();
		}

		std::vector< toolkit::MediaItem > items;

		do {
			if (stmt->dataType(3u) == core::Statement::Type::Null) {
				items.emplace_back(stmt->toInteger(0u), stmt->toText(1u), stmt->toText(2u));
			} else {
				items.emplace_back(stmt->toInteger(0u), stmt->toText(1u), stmt->toText(2u), stmt->toText(3u));
			}
		} while (stmt->nextRow());

		return items;
	}
}

namespace toolkit {
	MediaItem::MediaItem(long long id, std::string title, std::string uri, std::string thumbnail_file)
		: id_(id), title_(title), uri_(uri), thumbnail_(thumbnail_file) {}

	MediaItem::MediaItem(MediaItem const & media_item)
		: id_(media_item.id_), title_(media_item.title_), uri_(media_item.uri_), thumbnail_(media_item.thumbnail_) {}

	MediaItem::MediaItem(MediaItem && media_item)
		: id_(media_item.id_) {
		swap(title_, media_item.title_);
		swap(uri_, media_item.uri_);
		swap(thumbnail_, media_item.thumbnail_);
	}

/*
	Returns the database ID of the media item
*/
	long long MediaItem::id() const {
		return id_;
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
	std::string MediaItem::thumbnailFile() const {
		return thumbnail_;
	}

/*
	Initialises the library database
*/
	void Library::initialise_db() {
		dprint("Creating library database");
		clear();

		{
			core::Statement version_table(*this, "CREATE TABLE version (version INTEGER PRIMARY KEY)");
			assert(version_table.valid());
			version_table.execute();
		}

		{
			core::Statement set_version(*this, "INSERT INTO version (version) VALUES (?)");
			set_version.bind(1u, db_version);
			assert(set_version.valid());
			set_version.execute();
		}

		{
			core::Statement type_table(*this, "CREATE TABLE types "
					"(type_id INTEGER PRIMARY KEY AUTOINCREMENT, type TEXT NOT NULL)");
			assert(type_table.valid());
			type_table.execute();
		}

		{
			core::Statement set_types(*this, "INSERT INTO types (type) VALUES (?)");
			assert(set_types.valid());
			set_types.bind(1u, "movie");
			set_types.execute();
			set_types.reset();
			set_types.bind(1u, "music");
			set_types.execute();
		}

		{
			core::Statement albums_table(*this, "CREATE TABLE albums "
					"(album_id INTEGER PRIMARY KEY AUTOINCREMENT, album TEXT NOT NULL, thumbnail TEXT DEFAULT NULL)");
			assert(albums_table.valid());
			albums_table.execute();
		}

		{
			core::Statement items_table(*this, "CREATE TABLE items "
					"(item_id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, "
					"uri TEXT NOT NULL, thumbnail TEXT DEFAULT NULL, "
					"album_id REFERENCES albums (album_id) ON DELETE SET NULL, type_id REFERENCES types (type_id))");
			assert(items_table.valid());
			items_table.execute();
		}
	}

/*
	Find the foreign key relating to the given type
*/
	long long Library::type_key(Type type) {
		assert(type != Type::All);

		if (type_stmt_ == nullptr) {
			type_stmt_ = new core::Statement(*this, "SELECT key FROM types WHERE type = ?");
		} else {
			type_stmt_->reset();
		}

		switch (type) {
		case Type::Movies:
			type_stmt_->bind(1u, "movies");
			break;
		case Type::Music:
			type_stmt_->bind(1u, "music");
			break;
		default:
			assert(false);
		}

		assert(type_stmt_->valid());
		type_stmt_->execute();
		return type_stmt_->toInteger(0u);
	}

/*
	Find the foreign key relating to the given album
*/
	long long Library::album_key(std::string album) {
		if (album_stmt_ == nullptr) {
			album_stmt_ = new core::Statement(*this, "SELECT key FROM albums WHERE album = ?");
		} else {
			album_stmt_->reset();
		}

		album_stmt_->bind(1u, album);

		assert(album_stmt_->valid());
		album_stmt_->execute();
		return album_stmt_->toInteger(0u);
	}

	Library::Library()
		: core::Database(core::Path::data() + "/library.db"), add_stmt_(nullptr), count_stmt_(nullptr),
		  list_stmt_(nullptr), search_stmt_(nullptr) {
		core::Statement check_version(*this, "SELECT version FROM version");
		if (!check_version.valid()) {
			// Database is likely empty
			initialise_db();
		} else {
			check_version.execute();
			if (check_version.toInteger(0u) != db_version) {
				// Database is out of date
				dprint("Old database found");
				initialise_db();
			}
		}
	}

	Library::~Library() {
		delete add_stmt_;
		delete count_stmt_;
		delete list_stmt_;

		delete type_stmt_;
		delete album_stmt_;
	}

/*
	Adds a new entry to the library
*/
	void Library::add(std::string title, std::string uri, Library::Type type, std::string thumbnail_file, std::string album) {
		if (type == Type::All) {
			dprint("Trying to add media item with media type of 'All'");
			return;
		}

		if (add_stmt_ == nullptr) {
			add_stmt_ = new core::Statement(*this,
					"INSERT INTO items (name, uri, type, thumbnail, album) VALUES (?, ?, ?, ?, ?)");
		} else {
			add_stmt_->reset();
		}

		add_stmt_->bind(1u, title);
		add_stmt_->bind(2u, uri);
		add_stmt_->bind(3u, type_key(type));

		if (thumbnail_file.empty()) {
			add_stmt_->bind(4u);
		} else {
			add_stmt_->bind(4u, thumbnail_file);
		}

		if (album.empty()) {
			add_stmt_->bind(5u);
		} else {
			add_stmt_->bind(5u, album_key(album));
		}

		assert(add_stmt_->valid());
		add_stmt_->execute();
	}

/*
	Count the number of items in the media library of a given type
*/
	unsigned long long Library::count(Library::Type type) {
		if (count_stmt_ == nullptr) {
			count_stmt_ = new core::Statement(*this, "SELECT COUNT(item_id) FROM items WHERE type LIKE ?");
		} else {
			count_stmt_->reset();
		}

		count_stmt_->bind(1u, type_key(type));

		assert(count_stmt_->valid());
		count_stmt_->execute();
		assert(count_stmt_->hasData());

		return count_stmt_->toInteger(0u);
	}

/*
	Return the items of the given type from the media library
*/
	std::vector< MediaItem > Library::list(Library::Type type) {
		if (list_stmt_ == nullptr) {
			list_stmt_ = new core::Statement(*this,
					"SELECT item_id, name, uri, thumbnail FROM items NATURAL JOIN albums NATURAL JOIN types "
					"WHERE type LIKE ? ORDER BY album, name");
		} else {
			list_stmt_->reset();
		}

		switch (type) {
		case Type::All:
			list_stmt_->bind(1u, "%");
			break;
		case Type::Movies:
			list_stmt_->bind(1u, "movies");
			break;
		case Type::Music:
			list_stmt_->bind(1u, "music");
			break;
		}

		assert(list_stmt_->valid());
		list_stmt_->execute();

		return fetch(list_stmt_);
	}

/*
	Return the items of the given type from the media library that contain the search term
*/
	std::vector< MediaItem > Library::search(Library::Type type, std::string term) {
		if (search_stmt_ == nullptr) {
			search_stmt_ = new core::Statement(*this,
					"SELECT item_id, name, uri, thumbnail FROM items NATURAL JOIN albums NATURAL JOIN types "
					"WHERE type LIKE ? AND (name LIKE ?2 OR album LIKE ?2) ORDER BY album, name");
		} else {
			search_stmt_->reset();
		}


		switch (type) {
		case Type::All:
			list_stmt_->bind(1u, "%");
			break;
		case Type::Movies:
			list_stmt_->bind(1u, "movies");
			break;
		case Type::Music:
			list_stmt_->bind(1u, "music");
			break;
		}

		search_stmt_->bind(2u, "%" + term + "%");

		assert(search_stmt_->valid());
		search_stmt_->execute();

		return fetch(search_stmt_);
	}
}
