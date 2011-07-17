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
#include <toolkit/interface.hpp>

extern "C" {
#include <clutter-gst/clutter-gst.h>
}

namespace { namespace clutter {
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
}}

namespace toolkit {
	class InterfacePrivate
		: clutter::Initialiser {
	public:
		InterfacePrivate();

		void showAndRun() const;
	};

	InterfacePrivate::InterfacePrivate() {
			ClutterActor * default_stage = clutter_stage_get_default();
			clutter_stage_set_title(CLUTTER_STAGE(default_stage), NAME);
			clutter_stage_set_user_resizable(CLUTTER_STAGE(default_stage), TRUE);
	}

	void InterfacePrivate::showAndRun() const {
		clutter_actor_show(clutter_stage_get_default());
		clutter_main();
	}

	Interface::Interface()
		: p(new InterfacePrivate) {
		p->showAndRun();
	}

	Interface::~Interface() {
		delete p;
	}
}
