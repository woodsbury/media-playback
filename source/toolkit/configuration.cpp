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
#include <toolkit/configuration.hpp>

namespace {
	long long const db_version(1);
}

namespace toolkit {
	/*
		Initialises the configuration database
	*/
	void Configuration::initialise_db() {
		dprint("Creating configuration database");
		clear();
	}

	Configuration::Configuration()
		: core::Database(core::Path::data() + "/configuration.db") {
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

	Configuration::~Configuration() {}
}
