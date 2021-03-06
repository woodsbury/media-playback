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
#include <gst/gst.h>
}

#include "interface/interface_private.hpp"

namespace clutter {
	Initialiser::Initialiser() {
		if (!initialised_) {
			dprint("Initialising Clutter & GStreamer");
			clutter_gst_init(NULL, NULL);
			initialised_ = true;

#if defined(DEBUG) && !defined(GST_DISABLE_GST_DEBUG)
			gst_debug_set_active(TRUE);
			gst_debug_set_default_threshold(GST_LEVEL_ERROR);
#endif
		}
	}

	bool Initialiser::initialised_(false);
}

namespace toolkit {
	InterfacePrivate::InterfacePrivate()
		: browser_(this), player_(this) {
		clutter_stage_set_title(CLUTTER_STAGE(clutter_stage_get_default()), DISPLAY_NAME);
		clutter_stage_set_throttle_motion_events(CLUTTER_STAGE(clutter_stage_get_default()), TRUE);
		clutter_stage_set_user_resizable(CLUTTER_STAGE(clutter_stage_get_default()), TRUE);

		ClutterColor black = {0, 0, 0, 255};
		clutter_stage_set_color(CLUTTER_STAGE(clutter_stage_get_default()), &black);

		clutter_container_add_actor(CLUTTER_CONTAINER(clutter_stage_get_default()), browser_.actor());
		clutter_container_add_actor(CLUTTER_CONTAINER(clutter_stage_get_default()), player_.actor());
		clutter_container_add_actor(CLUTTER_CONTAINER(clutter_stage_get_default()), panel_.actor());

		clutter_actor_hide_all(player_.actor());
		browse();
	}

	/*
		Show the add file dialogue
	*/
	void InterfacePrivate::add() {}

	/*
		Browse the media library
	*/
	void InterfacePrivate::browse() {
		clutter_actor_detach_animation(browser_.actor());
		clutter_actor_animate(player_.actor(), CLUTTER_LINEAR, 250, "opacity", 0,
		                      "signal::completed", interface::Actor::hide_after_cb, player_.actor(), NULL);
		clutter_actor_show_all(browser_.actor());
		clutter_actor_animate(browser_.actor(), CLUTTER_LINEAR, 250, "opacity", 255, NULL);

		clutter_actor_grab_key_focus(browser_.actor());
		panel_.setAutoHide(false);
	}

	/*
		Informs the other widgets that the library has been updated
	*/
	void InterfacePrivate::libraryUpdated() {
		browser_.update();
	}

	/*
		Plays the URI
	*/
	void InterfacePrivate::play(std::string uri, std::string title) {
		clutter_actor_detach_animation(player_.actor());
		clutter_actor_animate(browser_.actor(), CLUTTER_LINEAR, 250, "opacity", 0,
		                      "signal::completed", interface::Actor::hide_after_cb, browser_.actor(), NULL);
		clutter_actor_show_all(player_.actor());
		clutter_actor_animate(player_.actor(), CLUTTER_LINEAR, 250, "opacity", 255, NULL);

		clutter_actor_grab_key_focus(player_.actor());
		panel_.setAutoHide(true);

		player_.play(std::move(uri), std::move(title));
	}

	Interface::Interface()
		: p(new InterfacePrivate) {}

	Interface::~Interface() {
		delete p;
	}

	void Interface::browse() const {
		p->browse();
	}

	void Interface::play(std::string uri, std::string title) const {
		p->play(std::move(uri), std::move(title));
	}

	/*
		Show the stage and start the main loop
	*/
	void Interface::start() const {
		clutter_actor_show(clutter_stage_get_default());
		clutter_main();
	}
}
