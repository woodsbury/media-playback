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
#include <glib.h>
}

namespace {
// Event callbacks
	gboolean quit_clicked(ClutterActor *, ClutterEvent * event, gpointer) {
		if (clutter_event_get_button(event) == 1) {
			// Left mouse button pressed
			dprint("Quitting");
			clutter_main_quit();
		}

		return TRUE;
	}

	gboolean fade_out(gpointer data) {
		clutter_state_set_state(CLUTTER_STATE(data), "fade-out");
		return FALSE;
	}

	gboolean fade_in(ClutterActor *, ClutterEvent *, gpointer data) {
		clutter_state_set_state(CLUTTER_STATE(data), "fade-in");
		g_timeout_add_seconds(2, fade_out, data);
		return TRUE;
	}

// Actors
	ClutterActor * create_play_area(ClutterActor * stage) {
		ClutterLayoutManager * layout = clutter_box_layout_new();
		ClutterActor * box = clutter_box_new(layout);
		clutter_actor_add_constraint(box, clutter_bind_constraint_new(stage, CLUTTER_BIND_SIZE, 0.0));

		ClutterActor * play = clutter_gst_video_texture_new();
		clutter_texture_set_keep_aspect_ratio(CLUTTER_TEXTURE(play), TRUE);
		clutter_box_pack(CLUTTER_BOX(box), play, "expand", TRUE, NULL);

		clutter_container_add_actor(CLUTTER_CONTAINER(stage), box);

		return play;
	}

	ClutterActor * create_quit_button(ClutterActor * stage) {
		// A quit button actor
		ClutterColor colour = {255, 255, 255, 255};
		ClutterActor * quit = clutter_text_new_full("Sans 12px", "Quit", &colour);
		clutter_actor_set_reactive(quit, true);
		g_signal_connect(quit, "button-press-event", G_CALLBACK(quit_clicked), NULL);
		clutter_actor_add_constraint(quit, clutter_align_constraint_new(stage, CLUTTER_ALIGN_X_AXIS, 0.99));
		clutter_actor_add_constraint(quit, clutter_align_constraint_new(stage, CLUTTER_ALIGN_Y_AXIS, 0.01));
		clutter_container_add_actor(CLUTTER_CONTAINER(stage), quit);

		return quit;
	}
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
		ClutterActor * play_area_;

	public:
		InterfacePrivate();

		void showAndRun() const;
	};

	InterfacePrivate::InterfacePrivate() {
		ClutterActor * stage = clutter_stage_get_default();
		clutter_stage_set_title(CLUTTER_STAGE(stage), NAME);
		clutter_stage_set_user_resizable(CLUTTER_STAGE(stage), TRUE);
		ClutterColor background = {0, 0, 0, 255};
		clutter_stage_set_color(CLUTTER_STAGE(stage), &background);

		play_area_ = create_play_area(stage);
		ClutterActor * quit_button = create_quit_button(stage);

		ClutterState * fade_state = clutter_state_new();
		clutter_state_set_duration(fade_state, NULL, NULL, 500);
		clutter_state_set(fade_state, NULL, "fade-out",
				quit_button, "opacity", CLUTTER_LINEAR, 0,
				NULL);
		clutter_state_set(fade_state, NULL, "fade-in",
				quit_button, "opacity", CLUTTER_LINEAR, 255,
				NULL);
		clutter_state_warp_to_state(fade_state, "fade-in");

		g_signal_connect(stage, "motion-event", G_CALLBACK(fade_in), fade_state);
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
