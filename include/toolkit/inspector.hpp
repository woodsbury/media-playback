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

#ifndef _TOOLKIT_INSPECTOR_HPP
#define _TOOLKIT_INSPECTOR_HPP

#include <string>
#include <core/noncopiable.hpp>

namespace toolkit {
	class InspectorPrivate;

	class Inspector
			: core::NonCopiable {
		InspectorPrivate * p;

	public:
		Inspector(std::string uri);
		~Inspector();

		bool audio() const;
		bool video() const;

		std::string album() const;
		std::string title() const;
	};
}

#endif
