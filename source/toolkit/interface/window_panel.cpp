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
	gboolean WindowPanel::close_clicked_cb(ClutterActor *, ClutterEvent *, gpointer) {
		clutter_main_quit();
		return TRUE;
	}

	gboolean WindowPanel::fullscreen_clicked_cb(ClutterActor *, ClutterEvent *, gpointer data) {
		reinterpret_cast< WindowPanel * >(data)->fullscreen_clicked();
		return TRUE;
	}

	void WindowPanel::fullscreen_status_changed_cb(ClutterStage *, gpointer data) {
		reinterpret_cast< WindowPanel * >(data)->draw_window_controls();
	}

	gboolean WindowPanel::hide_panel_cb(gpointer data) {
		reinterpret_cast< WindowPanel * >(data)->hide_panel();
		return FALSE;
	}

	gboolean WindowPanel::show_panel_cb(ClutterActor *, ClutterEvent *, gpointer data) {
		reinterpret_cast< WindowPanel * >(data)->show_panel();
		return FALSE;
	}

	WindowPanel::WindowPanel()
		: auto_hide_(false) {
		ClutterLayoutManager * main_layout = clutter_box_layout_new();
		clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(main_layout), 10u);
		actor_ = clutter_box_new(main_layout);
		clutter_actor_add_constraint(actor_, clutter_align_constraint_new(clutter_stage_get_default(),
				CLUTTER_ALIGN_X_AXIS, 0.95f));
		clutter_actor_add_constraint(actor_, clutter_bind_constraint_new(clutter_stage_get_default(),
				CLUTTER_BIND_Y, 5.0f));

		fullscreen_button_ = clutter_cairo_texture_new(18, 16);
		clutter_actor_set_reactive(fullscreen_button_, TRUE);
		g_signal_connect(fullscreen_button_, "button-press-event", G_CALLBACK(fullscreen_clicked_cb), this);
		g_signal_connect(fullscreen_button_, "enter-event", G_CALLBACK(actor_highlight_on_cb), fullscreen_button_);
		g_signal_connect(fullscreen_button_, "leave-event", G_CALLBACK(actor_highlight_off_cb), fullscreen_button_);
		clutter_box_pack(CLUTTER_BOX(actor_), fullscreen_button_, NULL, NULL);

		close_button_ = clutter_cairo_texture_new(15, 15);

		cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(close_button_));

		cairo_move_to(context, 1.0, 3.0);
		cairo_line_to(context, 3.0, 1.0);
		cairo_line_to(context, 8.0, 6.0);
		cairo_line_to(context, 13.0, 1.0);
		cairo_line_to(context, 15.0, 3.0);
		cairo_line_to(context, 10.0, 8.0);
		cairo_line_to(context, 15.0, 13.0);
		cairo_line_to(context, 13.0, 15.0);
		cairo_line_to(context, 8.0, 10.0);
		cairo_line_to(context, 3.0, 15.0);
		cairo_line_to(context, 1.0, 13.0);
		cairo_line_to(context, 6.0, 8.0);
		cairo_close_path(context);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_fill_preserve(context);
		cairo_set_line_width(context, 1.0);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke(context);

		cairo_destroy(context);

		clutter_actor_set_reactive(close_button_, TRUE);
		g_signal_connect(close_button_, "button-press-event", G_CALLBACK(close_clicked_cb), NULL);
		g_signal_connect(close_button_, "enter-event", G_CALLBACK(actor_highlight_on_cb), close_button_);
		g_signal_connect(close_button_, "leave-event", G_CALLBACK(actor_highlight_off_cb), close_button_);
		clutter_box_pack(CLUTTER_BOX(actor_), close_button_, NULL, NULL);

		draw_window_controls();

		g_signal_connect(clutter_stage_get_default(), "fullscreen", G_CALLBACK(fullscreen_status_changed_cb), this);
		g_signal_connect(clutter_stage_get_default(), "unfullscreen", G_CALLBACK(fullscreen_status_changed_cb), this);
		g_signal_connect(clutter_stage_get_default(), "motion-event", G_CALLBACK(show_panel_cb), this);
	}

/*
	Updates the window controls based on whether the stage is fullscreen
*/
	void WindowPanel::draw_window_controls() {
		clutter_cairo_texture_clear(CLUTTER_CAIRO_TEXTURE(fullscreen_button_));

		if (clutter_stage_get_fullscreen(CLUTTER_STAGE(clutter_stage_get_default()))) {
			// Stage is fullscreen
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

			clutter_actor_show(close_button_);
		} else {
			// Stage is windowed
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

			clutter_actor_hide(close_button_);
		}
	}

/*
	Called whenever the fullscreen button is clicked
*/
	void WindowPanel::fullscreen_clicked() {
		clutter_stage_set_fullscreen(CLUTTER_STAGE(clutter_stage_get_default()),
				!clutter_stage_get_fullscreen(CLUTTER_STAGE(clutter_stage_get_default())));
	}

/*
	Hides the panel
*/
	void WindowPanel::hide_panel() {
		if (clutter_actor_get_animation(actor_) != NULL) {
			clutter_actor_detach_animation(actor_);
		}

		clutter_actor_animate(actor_, CLUTTER_LINEAR, 500, "opacity", 0, NULL);
		hide_panel_timeout_id_ = 0;
	}

/*
	Shows the panel
*/
	void WindowPanel::show_panel() {
		if (clutter_actor_get_animation(actor_) != NULL) {
			return;
		}

		clutter_actor_animate(actor_, CLUTTER_LINEAR, 250, "opacity", 255, NULL);

		if (hide_panel_timeout_id_ != 0) {
			g_source_remove(hide_panel_timeout_id_);
		}

		if (auto_hide_) {
			hide_panel_timeout_id_ = g_timeout_add_seconds(3, hide_panel_cb, this);
		}
	}

/*
	Changes whether the panel hides itself after a timeout
*/
	void WindowPanel::setAutoHide(bool hide) {
		auto_hide_ = hide;

		if (hide) {
			hide_panel_timeout_id_ = g_timeout_add_seconds(3, hide_panel_cb, this);
		}
	}
}