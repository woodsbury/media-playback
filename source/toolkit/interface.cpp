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
#include <cairo.h>
#include <clutter-gst/clutter-gst.h>
#include <glib.h>
}

#include "interface/media_buttons.hpp"
#include "interface/play_area.hpp"
#include "interface/status_area.hpp"

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

	static ClutterStage * default_stage() {
		return CLUTTER_STAGE(clutter_stage_get_default());
	}
}}

namespace toolkit {
	class InterfacePrivate
		: clutter::Initialiser {
		clutter::PlayArea play_area_;
		clutter::MediaButtons media_buttons_;
		clutter::StatusArea status_area_;

		static gboolean fade_in(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean fade_out(gpointer data);

		static void width_changed(GObject * object, GParamSpec * param, gpointer data);

	public:
		InterfacePrivate();

		inline void play(char const * uri);

		inline void showAndRun() const;
	};

	gboolean InterfacePrivate::fade_in(ClutterActor *, ClutterEvent *, gpointer data) {
		static guint timeout_id(0);

		if (timeout_id > 0) {
			g_source_remove(timeout_id);
		}

		clutter_state_set_state(CLUTTER_STATE(data), "fade-in");
		timeout_id = g_timeout_add_seconds(2, fade_out, data);

		clutter_stage_show_cursor(clutter::default_stage());
		return TRUE;
	}

	gboolean InterfacePrivate::fade_out(gpointer data) {
		clutter_state_set_state(CLUTTER_STATE(data), "fade-out");
		clutter_stage_hide_cursor(clutter::default_stage());
		return FALSE;
	}

	void InterfacePrivate::width_changed(GObject * object, GParamSpec *, gpointer data) {
		reinterpret_cast< clutter::MediaButtons * >(data)->updateSeek(clutter_actor_get_width(CLUTTER_ACTOR(object)));
	}

	InterfacePrivate::InterfacePrivate()
		: play_area_(clutter::default_stage()),
		  media_buttons_(clutter::default_stage(), play_area_.media()),
		  status_area_(clutter::default_stage()) {
		ClutterActor * stage = clutter_stage_get_default();
		clutter_stage_set_title(CLUTTER_STAGE(stage), NAME);
		clutter_stage_set_user_resizable(CLUTTER_STAGE(stage), TRUE);
		ClutterColor background = {0, 0, 0, 255};
		clutter_stage_set_color(CLUTTER_STAGE(stage), &background);

		ClutterActor * status_area_actor = status_area_.actor();
		ClutterActor * media_buttons_actor = media_buttons_.actor();

		ClutterState * fade_state = clutter_state_new();
		clutter_state_set_duration(fade_state, NULL, NULL, 500);
		clutter_state_set(fade_state, NULL, "fade-out",
				status_area_actor, "opacity", CLUTTER_LINEAR, 0,
				media_buttons_actor, "opacity", CLUTTER_LINEAR, 0,
				NULL);
		clutter_state_set(fade_state, NULL, "fade-in",
				status_area_actor, "opacity", CLUTTER_LINEAR, 255,
				media_buttons_actor, "opacity", CLUTTER_LINEAR, 255,
				NULL);
		clutter_state_warp_to_state(fade_state, "fade-in");

		g_signal_connect(stage, "motion-event", G_CALLBACK(fade_in), fade_state);

		g_signal_connect(stage, "notify::width", G_CALLBACK(width_changed), &media_buttons_);
	}

	void InterfacePrivate::play(char const * uri) {
		clutter_media_set_uri(play_area_.media(), uri);
		clutter_media_set_playing(play_area_.media(), TRUE);
		media_buttons_.setPlaying(true);
	}

	void InterfacePrivate::showAndRun() const {
		clutter_actor_show(clutter_stage_get_default());
		clutter_main();
	}

	Interface::Interface()
		: p(new InterfacePrivate) {}

	Interface::~Interface() {
		delete p;
	}

	void Interface::play(std::string uri) const {
		p->play(uri.c_str());
	}

	void Interface::start() const {
		p->showAndRun();
	}
}
