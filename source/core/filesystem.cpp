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

#include <cstdlib>
#include <core/filesystem.hpp>

extern "C" {
#include <pwd.h>
}

namespace {
	char const separator('/');
	char const * current_dir = ".";
	char const * parent_dir = "..";

	void tokenise(std::vector< std::string > & tokens, std::string const input) {
		tokens.clear();

		std::string::size_type start_token = input.find_first_not_of(separator);
		std::string::size_type end_token = input.find_first_of(separator, start_token);

		while (start_token != input.npos) {
			if (end_token == input.npos) {
				end_token = input.length();
			}

			std::string token = input.substr(start_token, end_token - start_token);
			if (token == parent_dir) {
				tokens.pop_back();
			} else if (token != current_dir) {
				tokens.push_back(input.substr(start_token, end_token - start_token));
			}

			start_token = input.find_first_not_of(separator, end_token);
			end_token = input.find_first_of(separator, start_token);
		}
	}
}

namespace core {
	Path::Path(std::string path) {
		tokenise(path_, path);
	}

/*
	Convert the path to a string
*/
	std::string Path::toString() const {
		std::string result;

		for (std::vector< std::string >::const_iterator i = path_.begin(); i != path_.end(); ++i) {
			result.push_back(separator);
			result.append(*i);
		}

		if (result.empty()) {
			result.push_back(separator);
		}

		return result;
	}

/*
	Returns the application's data directory for this user
*/
	std::string Path::data() {
		return home() + "/." + NAME;
	}

/*
	Returns the current user's home directory
*/
	std::string Path::home() {
		char const * home_env = getenv("HOME");
		if (home_env != NULL) {
			return std::string(home_env);
		}

		passwd * home_pwd = getpwuid(getuid());
		return std::string(home_pwd->pw_dir);
	}
}
