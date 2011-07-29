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

#include <core/filesystem.hpp>
#include <toolkit/interface.hpp>

int main(int argc, char ** argv) {
	{
		core::Path data_path(core::Path::data());
		if (!data_path.exists()) {
			data_path.create();
		}
	}

	toolkit::Interface interface;

	if (argc > 1) {
		interface.play(argv[1]);
	}

	interface.start();

	return 0;
}
