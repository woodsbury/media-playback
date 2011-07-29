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

#ifndef _INTERFACE_BROWSER_HPP
#define _INTERFACE_BROWSER_HPP

#include <list>
#include <toolkit/library.hpp>

extern "C" {
#include <clutter/clutter.h>
}

#include "actor.hpp"

namespace toolkit {
	class InterfacePrivate;
}

namespace interface {
	class BrowserItem {
		toolkit::MediaItem item_;

		ClutterActor * actor_;
		ClutterContainer * list_;

	public:
		BrowserItem(toolkit::MediaItem media_item);
		~BrowserItem();

		ClutterActor * actor() const;

		void addToList(ClutterContainer * list);
		void removeFromList();
	};

	class Browser
		: public Actor {
		toolkit::InterfacePrivate * p;

		toolkit::Library library_;

		std::list< BrowserItem > item_list_;

		ClutterActor * media_list_;

		void clear_media_list();
		void update_media_list();

	public:
		Browser(toolkit::InterfacePrivate * interface_private);
	};
}

#endif
