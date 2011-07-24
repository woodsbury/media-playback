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

#ifndef _INTERFACE_PRIVATE_HPP
#define _INTERFACE_PRIVATE_HPP

extern "C" {
#include <clutter-gst/clutter-gst.h>
}

#include "player.hpp"
#include "window_panel.hpp"

namespace clutter {
/*
	Inheriting from this class will automatically initialise Clutter and GStreamer
*/
	class Initialiser {
		static bool initialised_;

	public:
		Initialiser() {
			if (!initialised_) {
				dprint("Initialising Clutter & GStreamer");
				clutter_gst_init(NULL, NULL);
				initialised_ = true;
			}
		}
	};

	bool Initialiser::initialised_(false);
}

namespace toolkit {
	class InterfacePrivate
		: clutter::Initialiser {
		interface::Player player;
		interface::WindowPanel panel;

	public:
		InterfacePrivate();

		inline void play(char const * uri);

		inline void start() const;
	};
}

#endif
