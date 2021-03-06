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

#ifndef _INTERFACE_PLAYER_HPP
#define _INTERFACE_PLAYER_HPP

extern "C" {
#include <clutter/clutter.h>
#include <gst/gst.h>
}

#include "actor.hpp"

namespace toolkit {
	class InterfacePrivate;
}

namespace interface {
	class Player
		: public Actor {
		static unsigned int const title_width;
		static unsigned int const title_height;

		static gboolean hide_controls_cb(gpointer data);
		static gboolean key_pressed_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static void media_eos_cb(ClutterMedia * media, gpointer data);
		static void media_error_cb(ClutterMedia * media, GError * error, gpointer data);
		static void media_message_cb(GstBus * bus, GstMessage * message, gpointer data);
		static gboolean next_clicked_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean play_clicked_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean previous_clicked_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean seek_dragged_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean show_controls_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean stop_clicked_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean update_seek_handle_cb(gpointer data);
		static gboolean volume_dragged_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static void width_changed_cb(GObject * object, GParamSpec * param, gpointer data);

		toolkit::InterfacePrivate * p;

		ClutterMedia * media_;

		ClutterActor * volume_;
		ClutterActor * volume_handle_;
		ClutterActor * volume_hidden_;

		ClutterActor * hud_;

		ClutterActor * title_;

		ClutterActor * play_button_;
		ClutterActor * seek_line_;
		ClutterActor * seek_handle_;
		ClutterActor * seek_hidden_;
		ClutterActor * previous_button_;
		ClutterActor * next_button_;

		unsigned int current_track_;
		unsigned int total_tracks_;

		guint update_seek_timeout_id_;
		guint hide_controls_timeout_id_;

		void set_title(std::string title);
		void set_volume(double volume);

		void draw_play_button();

		void hide_controls();
		void key_pressed(guint key, ClutterModifierType modifiers);
		void media_eos();
		void media_message(GstMessage * message);
		void next_clicked();
		void play_clicked();
		void previous_clicked();
		void seek_dragged(float x);
		void show_controls();
		void stop_clicked();
		void update_seek_handle();
		void volume_dragged(float x);
		void width_changed();

	public:
		Player(toolkit::InterfacePrivate * interface_private);

		void play(std::string uri, std::string title = std::string());
	};
}

#endif
