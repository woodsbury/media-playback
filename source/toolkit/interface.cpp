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

#include <toolkit/interface.hpp>

extern "C" {
#include <clutter/clutter.h>
}

#include "interface/interface_private.hpp"

namespace clutter {
	Initialiser::Initialiser() {
		if (!initialised_) {
			dprint("Initialising Clutter & GStreamer");
			clutter_gst_init(NULL, NULL);
			initialised_ = true;
		}
	}

	bool Initialiser::initialised_(false);
}

namespace toolkit {
	InterfacePrivate::InterfacePrivate()
		: browser(this), player(this) {
		clutter_stage_set_title(CLUTTER_STAGE(clutter_stage_get_default()), DISPLAY_NAME);
		clutter_stage_set_throttle_motion_events(CLUTTER_STAGE(clutter_stage_get_default()), TRUE);
		clutter_stage_set_user_resizable(CLUTTER_STAGE(clutter_stage_get_default()), TRUE);

		ClutterColor black = {0, 0, 0, 255};
		clutter_stage_set_color(CLUTTER_STAGE(clutter_stage_get_default()), &black);

		clutter_container_add_actor(CLUTTER_CONTAINER(clutter_stage_get_default()), browser.actor());
		clutter_container_add_actor(CLUTTER_CONTAINER(clutter_stage_get_default()), player.actor());
		clutter_container_add_actor(CLUTTER_CONTAINER(clutter_stage_get_default()), panel.actor());

		browse();
	}

/*
	Browse the media library
*/
	void InterfacePrivate::browse() {
		clutter_actor_show_all(browser.actor());
		clutter_actor_hide_all(player.actor());
		panel.setAutoHide(false);
	}

/*
	Plays the URI
*/
	void InterfacePrivate::play(char const * uri) {
		clutter_actor_show_all(player.actor());
		clutter_actor_hide_all(browser.actor());
		panel.setAutoHide(true);

		player.play(uri);
	}

/*
	Show the stage and start the main loop
*/
	void InterfacePrivate::start() const {
		clutter_actor_show(clutter_stage_get_default());
		clutter_main();
	}

	Interface::Interface()
		: p(new InterfacePrivate) {}

	Interface::~Interface() {
		delete p;
	}

	void Interface::browse() const {
		p->browse();
	}

	void Interface::play(std::string uri) const {
		p->play(uri.c_str());
	}

	void Interface::start() const {
		p->start();
	}
}
