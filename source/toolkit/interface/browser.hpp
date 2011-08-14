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

#ifndef _INTERFACE_BROWSER_HPP
#define _INTERFACE_BROWSER_HPP

#include <list>
#include <toolkit/library.hpp>

extern "C" {
#include <clutter/clutter.h>
}

#include "actor.hpp"

namespace toolkit {
	class InterfacePrivate;
}

namespace interface {
	class BrowserItem
		: public Actor {
		static gboolean item_clicked_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);

		toolkit::InterfacePrivate * p;

		toolkit::MediaItem item_;

		ClutterActor * thumbnail_;

		gulong clicked_handler_;

		void draw_thumbnail();

		void item_clicked();

	public:
		BrowserItem(toolkit::MediaItem media_item, toolkit::InterfacePrivate * interface_private);
		BrowserItem(BrowserItem const & browser_item);
		BrowserItem(BrowserItem && browser_item);
		~BrowserItem();
	};

	class Browser
		: public Actor {
		static gboolean add_clicked_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean all_clicked_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean movies_clicked_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean music_clicked_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static void height_changed_cb(GObject * object, GParamSpec * param, gpointer data);
		static gboolean key_pressed_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static gboolean scroll_dragged_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);
		static void search_activated_cb(ClutterText * text, gpointer data);
		static gboolean wheel_scrolled_cb(ClutterActor * actor, ClutterEvent * event, gpointer data);

		toolkit::InterfacePrivate * p;

		toolkit::Library library_;

		std::vector< BrowserItem > item_list_;

		toolkit::Library::Type type_;

		ClutterActor * all_;
		ClutterActor * music_;
		ClutterActor * movies_;

		ClutterActor * search_text_;

		ClutterActor * add_;

		ClutterActor * media_list_;

		ClutterActor * scroll_hidden_;
		ClutterActor * scroll_line_;
		ClutterActor * scroll_handle_;

		void draw_all_button();
		void draw_music_button();
		void draw_movies_button();

		void clear_media_list();
		void update_media_list();

		void add_clicked();
		void all_clicked();
		void key_pressed(guint key, ClutterModifierType modifiers);
		void movies_clicked();
		void music_clicked();
		void scroll_dragged(float y);
		void height_changed();
		void wheel_scrolled(bool up);

	public:
		Browser(toolkit::InterfacePrivate * interface_private);

		void update();
	};
}

#endif
