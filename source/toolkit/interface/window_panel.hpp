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

#ifndef _INTERFACE_WINDOW_PANEL_HPP
#define _INTERFACE_WINDOW_PANEL_HPP

extern "C" {
#include <clutter/clutter.h>
}

#include "actor.hpp"

namespace interface {
	class WindowPanel
		: public Actor {
		static gboolean close_clicked_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean fullscreen_clicked_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static void fullscreen_status_changed_cb(ClutterStage * stage, gpointer data);
		static gboolean hide_panel_cb(gpointer data);
		static gboolean show_panel_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);

		ClutterActor * close_button_;
		ClutterActor * fullscreen_button_;

		guint hide_panel_timeout_id_;

		bool auto_hide_;

		void draw_window_controls();

		void fullscreen_clicked();
		void hide_panel();
		void show_panel();

	public:
		WindowPanel();

		void setAutoHide(bool hide);
	};
}

#endif
