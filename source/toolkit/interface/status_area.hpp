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

namespace { namespace clutter {
	class StatusArea {
		ClutterActor * actor_;

		static gboolean fullscreen_clicked(ClutterActor *, ClutterEvent * event, gpointer data) {
		if (clutter_event_get_button(event) == 1) {
			// Left mouse button pressed
			dprint("Fullscreen clicked");
			clutter_stage_set_fullscreen(CLUTTER_STAGE(data), !clutter_stage_get_fullscreen(CLUTTER_STAGE(data)));
		}

		return TRUE;
	}

		static gboolean quit_clicked(ClutterActor *, ClutterEvent * event, gpointer) {
		if (clutter_event_get_button(event) == 1) {
			// Left mouse button pressed
			dprint("Quit clicked");
			clutter_main_quit();
		}

		return TRUE;
	}

	public:
		StatusArea(ClutterStage * stage) {
			ClutterColor white = {250, 250, 250, 200};

			ClutterLayoutManager * layout = clutter_box_layout_new();
			clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(layout), 10);
			actor_ = clutter_box_new(layout);
			clutter_actor_add_constraint(actor_, clutter_align_constraint_new(CLUTTER_ACTOR(stage), CLUTTER_ALIGN_X_AXIS, 0.99));
			clutter_actor_add_constraint(actor_, clutter_align_constraint_new(CLUTTER_ACTOR(stage), CLUTTER_ALIGN_Y_AXIS, 0.01));

			// A fullscreen button actor
			ClutterActor * fullscreen = clutter_text_new_full("Sans 12px", "Fullscreen", &white);
			clutter_actor_set_reactive(fullscreen, TRUE);
			g_signal_connect(fullscreen, "button-press-event", G_CALLBACK(fullscreen_clicked), stage);
			clutter_box_pack(CLUTTER_BOX(actor_), fullscreen, NULL, NULL);

			// A quit button actor
			ClutterActor * quit = clutter_text_new_full("Sans 12px", "Quit", &white);
			clutter_actor_set_reactive(quit, TRUE);
			g_signal_connect(quit, "button-press-event", G_CALLBACK(quit_clicked), NULL);
			clutter_box_pack(CLUTTER_BOX(actor_), quit, NULL, NULL);

			clutter_container_add_actor(CLUTTER_CONTAINER(stage), actor_);
		}

		ClutterActor * actor() const {
			return actor_;
		}
	};
}}
