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
}

#include "actor.hpp"

namespace interface {
	class Player
		: public Actor {
		static gboolean hide_controls_cb(gpointer data);
		static void media_eos_cb(ClutterMedia * media, gpointer data);
		static gboolean play_clicked_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean seek_dragged_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean show_controls_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean update_seek_handle_cb(gpointer data);
		static void width_changed_cb(GObject * object, GParamSpec * param, gpointer data);


		ClutterMedia * media_;

		ClutterActor * controls_;

		ClutterActor * play_button_;
		ClutterActor * seek_line_;
		ClutterActor * seek_handle_;
		ClutterActor * seek_hidden_;

		guint update_seek_timeout_id_;
		guint hide_controls_timeout_id_;

		void draw_play_button();

		void hide_controls();
		void media_eos();
		void play_clicked();
		void seek_dragged(float x);
		void show_controls();
		void update_seek_handle();
		void width_changed();

	public:
		Player();

		void play(char const * uri);
	};
}

#endif
