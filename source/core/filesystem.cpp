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
#include <unistd.h>
}

namespace {
	char const separator('/');
	char const * current_dir = ".";
	char const * parent_dir = "..";
}

namespace core {
	Path::Path(std::string const path) {
		set(path);
	}

/*
	Set the path to a new location
*/
	void Path::set(std::string const path) {
		path_.clear();

		if (path.empty()) {
			absolute_ = true;
			return;
		} else if (path[0] == separator) {
			absolute_ = true;
		} else {
			absolute_ = false;
		}

		std::string::size_type start_token = path.find_first_not_of(separator);
		std::string::size_type end_token = path.find_first_of(separator, start_token);

		while (start_token != path.npos) {
			if (end_token == path.npos) {
				end_token = path.length();
			}

			std::string token = path.substr(start_token, end_token - start_token);
			if ((token == parent_dir) && (path_.size() > 1)) {
				path_.pop_back();
			} else if (token != current_dir) {
				path_.push_back(path.substr(start_token, end_token - start_token));
			}

			start_token = path.find_first_not_of(separator, end_token);
			end_token = path.find_first_of(separator, start_token);
		}
	}

/*
	Check if the path is absolute or relative
*/
	bool Path::absolute() const {
		return absolute_;
	}

/*
	Check if the path exists
*/
	bool Path::exists() const {
		return exists(toString());
	}

/*
	Change the path to being absolute based on the current working directory
*/
	bool Path::makeAbsolute() {
		if (absolute_) {
			return true;
		}

		std::string current_directory = current();
		if (current_directory.empty()) {
			return false;
		}

		set(current() + "/" + toString());
		return true;
	}

/*
	Convert the path to a string
*/
	std::string Path::toString() const {
		std::string result;

		if (absolute_) {
			result.push_back(separator);
		}

		std::vector< std::string >::const_iterator i = path_.begin();

		if (i != path_.end()) {
			result.append(*i);
			++i;
		}

		for (; i != path_.end(); ++i) {
			result.push_back(separator);
			result.append(*i);
		}

		return result;
	}

/*
	Returns the path to the current working directory
*/
	std::string Path::current() {
		size_t size = 256;
		char * buffer = static_cast< char * >(std::malloc(size));

		// Use a buffer up to 2^5 * 256
		for (unsigned int i = 0; i < 5; ++i) {
			char * temp = getcwd(buffer, size);

			if (temp != nullptr) {
				std::string path(buffer);
				std::free(buffer);
				return path;
			}

			if (errno != ERANGE) {
				std::free(buffer);
				return std::string();
			}

			size = 2 * size;
			temp = static_cast< char * >(std::realloc(buffer, size));

			if (temp == nullptr) {
				std::free(buffer);
				return std::string();
			}

			buffer = temp;
		}

		std::free(buffer);
		return std::string();
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

/*
	Check if the given path exists
*/
	bool Path::exists(std::string const path) {
		return access(path.c_str(), F_OK) == 0;
	}
}
