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
			ClutterLayoutManager * layout = clutter_box_layout_new();
			clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(layout), 10);
			actor_ = clutter_box_new(layout);
			clutter_actor_add_constraint(actor_, clutter_align_constraint_new(CLUTTER_ACTOR(stage), CLUTTER_ALIGN_X_AXIS, 0.99));
			clutter_actor_add_constraint(actor_, clutter_align_constraint_new(CLUTTER_ACTOR(stage), CLUTTER_ALIGN_Y_AXIS, 0.01));

			// A fullscreen button actor
			ClutterActor * fullscreen = clutter_cairo_texture_new(18, 18);

			cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(fullscreen));

			cairo_rectangle(context, 2.0, 2.0, 14.0, 14.0);
			cairo_rectangle(context, 4.0, 4.0, 10.0, 10.0);
			cairo_set_fill_rule(context, CAIRO_FILL_RULE_EVEN_ODD);
			cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
			cairo_fill_preserve(context);
			cairo_set_line_width(context, 1.0);
			cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
			cairo_stroke(context);

			cairo_destroy(context);

			clutter_actor_set_reactive(fullscreen, TRUE);
			g_signal_connect(fullscreen, "button-press-event", G_CALLBACK(fullscreen_clicked), stage);
			clutter_box_pack(CLUTTER_BOX(actor_), fullscreen, NULL, NULL);

			// A quit button actor
			ClutterActor * quit = clutter_cairo_texture_new(18, 18);

			context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(quit));

			cairo_move_to(context, 2.0, 4.0);
			cairo_line_to(context, 14.0, 16.0);
			cairo_line_to(context, 16.0, 14.0);
			cairo_line_to(context, 4.0, 2.0);
			cairo_line_to(context, 2.0, 4.0);
			cairo_move_to(context, 14.0, 2.0);
			cairo_line_to(context, 16.0, 4.0);
			cairo_line_to(context, 4.0, 16.0);
			cairo_line_to(context, 2.0, 14.0);
			cairo_line_to(context, 14.0, 2.0);
			cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
			cairo_fill_preserve(context);
			cairo_set_line_width(context, 1.0);
			cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
			cairo_stroke(context);

			cairo_destroy(context);

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
