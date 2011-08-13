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

namespace toolkit {
	Configuration::Configuration()
		: core::Database(core::Path::data() + "/configuration.db") {
		if (opened() && (tables().size() == 0u)) {
			// Configuration database hasn't been initialised yet
			dprint("Creating configuration");
		}
	}

	Configuration::~Configuration() {}
}
