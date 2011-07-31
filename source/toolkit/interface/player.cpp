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

extern "C" {
#include <cairo.h>
#include <clutter-gst/clutter-gst.h>
}

#include "interface_private.hpp"
#include "player.hpp"

namespace interface {
	unsigned int const Player::title_width(300);
	unsigned int const Player::title_height(30);

	gboolean Player::hide_controls_cb(gpointer data) {
		reinterpret_cast< Player * >(data)->hide_controls();
		return FALSE;
	}

	gboolean Player::key_pressed_cb(ClutterActor *, ClutterEvent * event, gpointer data) {
		reinterpret_cast< Player * >(data)->key_pressed(clutter_event_get_key_symbol(event),
				clutter_event_get_state(event));
		return TRUE;
	}

	void Player::media_eos_cb(ClutterMedia *, gpointer data) {
		reinterpret_cast< Player * >(data)->media_eos();
	}

	gboolean Player::play_clicked_cb(ClutterActor *, ClutterEvent * event, gpointer data) {
		if (clutter_event_get_button(event) == 1) {
			reinterpret_cast< Player * >(data)->play_clicked();
		}

		return TRUE;
	}

	gboolean Player::seek_dragged_cb(ClutterActor *, ClutterEvent * event, gpointer data) {
		bool dragged = false;

		switch (clutter_event_type(event)) {
		case CLUTTER_BUTTON_PRESS:
			if (clutter_event_get_button(event) == 1) {
				dragged = true;
			}
			break;
		case CLUTTER_MOTION:
			if ((clutter_event_get_state(event) & CLUTTER_BUTTON1_MASK) != 0) {
				dragged = true;
			}
			break;
		default:
			;
		}

		if (dragged) {
			gfloat x_click;
			clutter_event_get_coords(event, &x_click, NULL);

			reinterpret_cast< Player * >(data)->seek_dragged(x_click);
		}

		return TRUE;
	}

	gboolean Player::show_controls_cb(ClutterActor *, ClutterEvent *, gpointer data) {
		reinterpret_cast< Player * >(data)->show_controls();
		return FALSE;
	}

	gboolean Player::stop_clicked_cb(ClutterActor *, ClutterEvent *, gpointer data) {
		reinterpret_cast< Player * >(data)->stop_clicked();
		return TRUE;
	}

	gboolean Player::update_seek_handle_cb(gpointer data) {
		reinterpret_cast< Player * >(data)->update_seek_handle();
		return TRUE;
	}

	void Player::width_changed_cb(GObject *, GParamSpec *, gpointer data) {
		reinterpret_cast< Player * >(data)->width_changed();
	}

	Player::Player(toolkit::InterfacePrivate * interface_private)
		: p(interface_private), update_seek_timeout_id_(0) {
		ClutterLayoutManager * main_layout = clutter_bin_layout_new(CLUTTER_BIN_ALIGNMENT_FIXED,
				CLUTTER_BIN_ALIGNMENT_FIXED);
		actor_ = clutter_box_new(main_layout);
		clutter_actor_add_constraint(actor_, clutter_bind_constraint_new(clutter_stage_get_default(),
				CLUTTER_BIND_SIZE, 0.0f));

		media_ = CLUTTER_MEDIA(clutter_gst_video_texture_new());
		clutter_texture_set_keep_aspect_ratio(CLUTTER_TEXTURE(media_), TRUE);
		clutter_actor_add_constraint(CLUTTER_ACTOR(media_),
				clutter_align_constraint_new(clutter_stage_get_default(), CLUTTER_ALIGN_Y_AXIS, 0.5f));
		g_signal_connect(media_, "eos", G_CALLBACK(media_eos_cb), this);
		clutter_box_pack(CLUTTER_BOX(actor_), CLUTTER_ACTOR(media_), NULL, NULL);

		ClutterLayoutManager * hud_layout = clutter_box_layout_new();
		clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(hud_layout), 7u);
		clutter_box_layout_set_vertical(CLUTTER_BOX_LAYOUT(hud_layout), TRUE);
		hud_ = clutter_box_new(hud_layout);
		clutter_actor_add_constraint(hud_,
				clutter_align_constraint_new(clutter_stage_get_default(), CLUTTER_ALIGN_X_AXIS, 0.5f));
		clutter_actor_add_constraint(hud_,
				clutter_align_constraint_new(clutter_stage_get_default(), CLUTTER_ALIGN_Y_AXIS, 0.93f));

		title_ = clutter_cairo_texture_new(title_width, title_height);
		clutter_box_pack(CLUTTER_BOX(hud_), title_, NULL, NULL);

		ClutterLayoutManager * controls_layout = clutter_box_layout_new();
		clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(controls_layout), 8u);
		ClutterActor * controls = clutter_box_new(controls_layout);

		play_button_ = clutter_cairo_texture_new(20, 20);
		draw_play_button();
		clutter_actor_set_reactive(play_button_, TRUE);
		g_signal_connect(play_button_, "button-press-event", G_CALLBACK(play_clicked_cb), this);
		g_signal_connect(play_button_, "enter-event", G_CALLBACK(actor_highlight_on_cb),
				play_button_);
		g_signal_connect(play_button_, "leave-event", G_CALLBACK(actor_highlight_off_cb),
				play_button_);
		clutter_box_pack(CLUTTER_BOX(controls), play_button_, NULL, NULL);

		ClutterActor * stop_button = clutter_cairo_texture_new(20, 20);

		cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(stop_button));

		cairo_rectangle(context, 2.0, 2.0, 16.0, 16.0);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_fill_preserve(context);
		cairo_set_line_width(context, 1.0);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke(context);

		cairo_destroy(context);

		clutter_actor_set_reactive(stop_button, TRUE);
		g_signal_connect(stop_button, "button-press-event", G_CALLBACK(stop_clicked_cb), this);
		g_signal_connect(stop_button, "enter-event", G_CALLBACK(actor_highlight_on_cb),
				stop_button);
		g_signal_connect(stop_button, "leave-event", G_CALLBACK(actor_highlight_off_cb),
				stop_button);
		clutter_box_pack(CLUTTER_BOX(controls), stop_button, NULL, NULL);

		ClutterActor * spacing = clutter_rectangle_new();
		clutter_actor_set_width(spacing, 2.0f);
		clutter_actor_set_opacity(spacing, 0);
		clutter_box_pack(CLUTTER_BOX(controls), spacing, NULL, NULL);

		ClutterLayoutManager * seek_layout = clutter_bin_layout_new(CLUTTER_BIN_ALIGNMENT_FIXED,
				CLUTTER_BIN_ALIGNMENT_CENTER);
		ClutterActor * seek = clutter_box_new(seek_layout);

		seek_hidden_ = clutter_rectangle_new();
		clutter_actor_set_height(seek_hidden_, 10.0f);
		clutter_actor_set_opacity(seek_hidden_, 0);
		clutter_actor_set_reactive(seek_hidden_, TRUE);
		g_signal_connect(seek_hidden_, "button-press-event", G_CALLBACK(seek_dragged_cb), this);
		g_signal_connect(seek_hidden_, "motion-event", G_CALLBACK(seek_dragged_cb), this);
		clutter_box_pack(CLUTTER_BOX(seek), seek_hidden_, NULL, NULL);

		ClutterColor black = {0, 0, 0, 255};
		ClutterColor white = {255, 255, 255, 255};
		seek_line_ = clutter_rectangle_new_with_color(&white);
		clutter_rectangle_set_border_color(CLUTTER_RECTANGLE(seek_line_), &black);
		clutter_rectangle_set_border_width(CLUTTER_RECTANGLE(seek_line_), 1);
		clutter_actor_set_height(seek_line_, 4.0f);
		clutter_box_pack(CLUTTER_BOX(seek), seek_line_, NULL, NULL);

		seek_handle_ = clutter_cairo_texture_new(12, 10);

		context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(seek_handle_));

		cairo_rectangle(context, 1.0, 1.0, 10.0, 8.0);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_fill_preserve(context);
		cairo_set_line_width(context, 1.0);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke(context);

		cairo_destroy(context);

		g_signal_connect(seek_hidden_, "enter-event", G_CALLBACK(actor_highlight_on_cb),
				seek_handle_);
		g_signal_connect(seek_hidden_, "leave-event", G_CALLBACK(actor_highlight_off_cb),
				seek_handle_);
		clutter_box_pack(CLUTTER_BOX(seek), seek_handle_, NULL, NULL);

		clutter_box_pack(CLUTTER_BOX(controls), seek, NULL, NULL);

		clutter_box_pack(CLUTTER_BOX(hud_), controls, NULL, NULL);

		clutter_box_pack(CLUTTER_BOX(actor_), hud_, NULL, NULL);

		g_signal_connect(actor_, "key-press-event", G_CALLBACK(key_pressed_cb), this);

		g_signal_connect(clutter_stage_get_default(), "notify::width", G_CALLBACK(width_changed_cb), this);
		g_signal_connect(clutter_stage_get_default(), "motion-event", G_CALLBACK(show_controls_cb), this);
		hide_controls_timeout_id_ = g_timeout_add_seconds(3, hide_controls_cb, this);
	}

/*
	Updates the play button based on the state of the media widget
*/
	void Player::draw_play_button() {
		clutter_cairo_texture_clear(CLUTTER_CAIRO_TEXTURE(play_button_));
		cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(play_button_));

		if (clutter_media_get_playing(media_)) {
			// Playing, draw pause button
			cairo_rectangle(context, 4.0, 2.0, 5.0, 16.0);
			cairo_rectangle(context, 12.0, 2.0, 5.0, 16.0);
			cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
			cairo_fill_preserve(context);
			cairo_set_line_width(context, 1.0);
			cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
			cairo_stroke(context);

			if (update_seek_timeout_id_ != 0) {
				g_source_remove(update_seek_timeout_id_);
			}
			update_seek_timeout_id_ = g_timeout_add(250, update_seek_handle_cb, this);
		} else {
			// Not playing, draw play button
			cairo_move_to(context, 2.0, 1.0);
			cairo_line_to(context, 2.0, 19.0);
			cairo_line_to(context, 18.0, 10.0);
			cairo_close_path(context);

			cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
			cairo_fill_preserve(context);
			cairo_set_line_width(context, 1.0);
			cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
			cairo_stroke(context);

			if (update_seek_timeout_id_ != 0) {
				g_source_remove(update_seek_timeout_id_);
				update_seek_timeout_id_ = 0;
			}
		}

		cairo_destroy(context);
	}

/*
	Hides the buttons and seek bar
*/
	void Player::hide_controls() {
		if (clutter_actor_get_animation(hud_) != NULL) {
			clutter_actor_detach_animation(hud_);
		}

		clutter_actor_animate(hud_, CLUTTER_LINEAR, 500, "opacity", 0, NULL);
		hide_controls_timeout_id_ = 0;

		clutter_stage_hide_cursor(CLUTTER_STAGE(clutter_stage_get_default()));
	}

/*
	Called whenever a key is pressed
*/
	void Player::key_pressed(guint key, ClutterModifierType modifiers) {
		switch (key) {
		case CLUTTER_KEY_space:
			// Emulate play click
			play_clicked();
			break;
		default:
			;
		}
	}

/*
	Called when the media playing back ends
*/
	void Player::media_eos() {
		draw_play_button();
		p->browse();
	}

/*
	Called whenever the play button is clicked
*/
	void Player::play_clicked() {
		gchar * uri = clutter_media_get_uri(media_);

		if (uri != NULL) {
			clutter_media_set_playing(media_, !clutter_media_get_playing(media_));
			draw_play_button();
			g_free(uri);
		}
	}

/*
	Called whenever the seek handle is dragged
*/
	void Player::seek_dragged(float x) {
		gfloat x_line;
		clutter_actor_get_transformed_position(seek_line_, &x_line, NULL);
		gfloat line_width = clutter_actor_get_width(seek_line_);

		gfloat progress = (x - x_line) / line_width;
		clutter_media_set_progress(media_, progress);

		update_seek_handle();
	}

/*
	Shows the buttons and seek bar
*/
	void Player::show_controls() {
		if (clutter_actor_get_animation(hud_) != NULL) {
			return;
		}

		clutter_actor_animate(hud_, CLUTTER_LINEAR, 250, "opacity", 255, NULL);

		if (hide_controls_timeout_id_ != 0) {
			g_source_remove(hide_controls_timeout_id_);
		}
		hide_controls_timeout_id_ = g_timeout_add_seconds(3, hide_controls_cb, this);

		clutter_stage_show_cursor(CLUTTER_STAGE(clutter_stage_get_default()));
	}

/*
	Called whenever the stop button is clicked
*/
	void Player::stop_clicked() {
		clutter_media_set_playing(media_, FALSE);
		p->browse();
	}

/*
	Called when the seek handle needs to be moved
*/
	void Player::update_seek_handle() {
		clutter_actor_set_x(seek_handle_,
				(clutter_actor_get_width(seek_line_) - clutter_actor_get_width(seek_handle_)) *
				clutter_media_get_progress(media_));
	}

/*
	Called whenever the stage's width changes
*/
	void Player::width_changed() {
		clutter_actor_set_width(seek_line_, clutter_actor_get_width(clutter_stage_get_default()) * 0.75f);
		clutter_actor_set_width(seek_hidden_, clutter_actor_get_width(clutter_stage_get_default()) * 0.75f);
	}

/*
	Plays the given URI in the media widget
*/
	void Player::play(char const * uri, char const * title) {
		clutter_cairo_texture_clear(CLUTTER_CAIRO_TEXTURE(title_));
		cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(title_));

		cairo_select_font_face(context, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(context, 18.0);

		cairo_text_extents_t extents;
		cairo_text_extents(context, title, &extents);
		cairo_move_to(context, (title_width / 2.0) - ((extents.width / 2.0) + extents.x_bearing),
				(title_height / 2.0) - ((extents.height / 2.0) + extents.y_bearing));

		cairo_text_path(context, title);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_fill_preserve(context);
		cairo_set_line_width(context, 1.0);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke(context);

		cairo_destroy(context);

		// Clear the image in the video texture
		unsigned char blank_data[3] = {0, 0, 0};
		CoglHandle blank = cogl_texture_new_from_data(1, 1, COGL_TEXTURE_NONE, COGL_PIXEL_FORMAT_RGB_888, COGL_PIXEL_FORMAT_ANY, 0, blank_data);
		clutter_texture_set_cogl_texture(CLUTTER_TEXTURE(media_), blank);
		cogl_handle_unref(blank);

		clutter_media_set_uri(media_, uri);
		clutter_media_set_playing(media_, TRUE);
		draw_play_button();
	}
}
