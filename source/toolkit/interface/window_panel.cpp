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

#include "window_panel.hpp"

namespace interface {
	gboolean WindowPanel::fullscreen_clicked_cb(ClutterActor *, ClutterEvent *, gpointer data) {
		reinterpret_cast< WindowPanel * >(data)->fullscreen_clicked();
		return TRUE;
	}

	void WindowPanel::fullscreen_status_changed_cb(ClutterStage *, gpointer data) {
		reinterpret_cast< WindowPanel * >(data)->draw_window_controls();
	}

	WindowPanel::WindowPanel() {
		ClutterLayoutManager * main_layout = clutter_box_layout_new();
		clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(main_layout), 10);
		actor_ = clutter_box_new(main_layout);
		clutter_actor_add_constraint(actor_, clutter_align_constraint_new(clutter_stage_get_default(),
				CLUTTER_ALIGN_X_AXIS, 0.95f));
		clutter_actor_add_constraint(actor_, clutter_bind_constraint_new(clutter_stage_get_default(),
				CLUTTER_BIND_Y, 5.0f));

		fullscreen_button_ = clutter_cairo_texture_new(18, 16);
		clutter_actor_set_reactive(fullscreen_button_, TRUE);
		g_signal_connect(fullscreen_button_, "button-press-event", G_CALLBACK(fullscreen_clicked_cb), this);
		clutter_box_pack(CLUTTER_BOX(actor_), fullscreen_button_, NULL, NULL);

		draw_window_controls();

		g_signal_connect(clutter_stage_get_default(), "fullscreen", G_CALLBACK(fullscreen_status_changed_cb), this);
		g_signal_connect(clutter_stage_get_default(), "unfullscreen", G_CALLBACK(fullscreen_status_changed_cb), this);
	}

/*
	Updates the window controls based on whether the stage is fullscreen
*/
	void WindowPanel::draw_window_controls() {
		clutter_cairo_texture_clear(CLUTTER_CAIRO_TEXTURE(fullscreen_button_));

		if (clutter_stage_get_fullscreen(CLUTTER_STAGE(clutter_stage_get_default()))) {
			cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(fullscreen_button_));

			cairo_rectangle(context, 1.0, 1.0, 16.0, 14.0);
			cairo_rectangle(context, 4.0, 6.0, 10.0, 6.0);
			cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
			cairo_set_fill_rule(context, CAIRO_FILL_RULE_EVEN_ODD);
			cairo_fill_preserve(context);
			cairo_set_line_width(context, 1.0);
			cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
			cairo_stroke(context);

			cairo_destroy(context);
		} else {
			cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(fullscreen_button_));

			cairo_rectangle(context, 1.0, 1.0, 16.0, 14.0);
			cairo_rectangle(context, 4.0, 4.0, 10.0, 8.0);
			cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
			cairo_set_fill_rule(context, CAIRO_FILL_RULE_EVEN_ODD);
			cairo_fill_preserve(context);
			cairo_set_line_width(context, 1.0);
			cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
			cairo_stroke(context);

			cairo_destroy(context);
		}
	}

/*
	Called whenever the fullscreen button is clicked
*/
	void WindowPanel::fullscreen_clicked() {
		clutter_stage_set_fullscreen(CLUTTER_STAGE(clutter_stage_get_default()),
				!clutter_stage_get_fullscreen(CLUTTER_STAGE(clutter_stage_get_default())));
	}
}
