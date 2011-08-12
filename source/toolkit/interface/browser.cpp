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

#include <algorithm>
#include <cmath>
#include <debug.hpp>
#include <core/filesystem.hpp>

#include "browser.hpp"
#include "interface_private.hpp"

namespace interface {
	gboolean BrowserItem::item_clicked_cb(ClutterActor *, ClutterEvent *, gpointer data) {
		reinterpret_cast< BrowserItem * >(data)->item_clicked();
		return TRUE;
	}

	BrowserItem::BrowserItem(toolkit::MediaItem media_item, toolkit::InterfacePrivate * interface_private)
		: p(interface_private), item_(std::move(media_item)) {
		ClutterLayoutManager * item_layout = clutter_box_layout_new();
		clutter_box_layout_set_vertical(CLUTTER_BOX_LAYOUT(item_layout), TRUE);
		actor_ = clutter_box_new(item_layout);
		clutter_actor_set_reactive(actor_, TRUE);
		clicked_handler_ = g_signal_connect(actor_, "button-press-event", G_CALLBACK(item_clicked_cb), this);

		thumbnail_ = clutter_cairo_texture_new(180, 180);
		draw_thumbnail();
		g_signal_connect(actor_, "enter-event", G_CALLBACK(Actor::actor_scale_on_cb), thumbnail_);
		g_signal_connect(actor_, "leave-event", G_CALLBACK(Actor::actor_scale_off_cb), thumbnail_);
		clutter_box_pack(CLUTTER_BOX(actor_), thumbnail_, NULL, NULL);

		ClutterColor white = {255, 255, 255, 255};
		ClutterActor * title = clutter_text_new_full("Sans", item_.title().c_str(), &white);
		g_signal_connect(actor_, "enter-event", G_CALLBACK(Actor::actor_highlight_on_cb), title);
		g_signal_connect(actor_, "leave-event", G_CALLBACK(Actor::actor_highlight_off_cb), title);
		clutter_box_pack(CLUTTER_BOX(actor_), title, NULL, NULL);

		g_object_ref_sink(actor_);
	}

	BrowserItem::BrowserItem(BrowserItem const & browser_item)
		: p(browser_item.p), item_(browser_item.item_), thumbnail_(browser_item.thumbnail_) {
		g_object_ref(browser_item.actor_);
		actor_ = browser_item.actor_;
		g_signal_handler_disconnect(actor_, browser_item.clicked_handler_);
		clicked_handler_ = g_signal_connect(actor_, "button-press-event", G_CALLBACK(item_clicked_cb), this);
	}

	BrowserItem::BrowserItem(BrowserItem && browser_item)
		: p(browser_item.p), item_(std::move(browser_item.item_)), thumbnail_(browser_item.thumbnail_) {
		g_object_ref(browser_item.actor_);
		actor_ = browser_item.actor_;
		g_signal_handler_disconnect(actor_, browser_item.clicked_handler_);
		clicked_handler_ = g_signal_connect(actor_, "button-press-event", G_CALLBACK(item_clicked_cb), this);
	}

	BrowserItem::~BrowserItem() {
		g_object_unref(actor_);
	}

/*
	Renders the thumnail for the item
*/
	void BrowserItem::draw_thumbnail() {
		cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(thumbnail_));

		if (!item_.thumbnailFile().empty() && core::Path::exists(item_.thumbnailFile())) {
			cairo_surface_t * image = cairo_image_surface_create_from_png(item_.thumbnailFile().c_str());

			double scale_factor = std::min(176.0 / cairo_image_surface_get_width(image),
					99.0 / cairo_image_surface_get_height(image));
			cairo_translate(context, 90.0 - ((cairo_image_surface_get_width(image) * scale_factor) / 2.0),
					90.0 - ((cairo_image_surface_get_height(image) * scale_factor) / 2.0));
			cairo_scale(context, scale_factor, scale_factor);

			cairo_set_source_surface(context, image, 0.0, 0.0);
			cairo_paint(context);

			cairo_surface_destroy(image);
		} else {
			// Draw generic thumbnail
			cairo_rectangle(context, 1.0, 1.0, 178.0, 178.0);

			cairo_set_source_rgba(context, 0.3, 0.3, 0.4, 0.4);
			cairo_fill(context);
		}

		cairo_destroy(context);
	}

/*
	Called whenever the item is clicked
*/
	void BrowserItem::item_clicked() {
		p->play(item_.uri().c_str(), item_.title().c_str());
	}

	gboolean Browser::add_clicked_cb(ClutterActor *, ClutterEvent * event, gpointer data) {
		if (clutter_event_get_button(event) == 1) {
			reinterpret_cast< Browser * >(data)->add_clicked();
		}

		return TRUE;
	}

	gboolean Browser::all_clicked_cb(ClutterActor *, ClutterEvent * event, gpointer data) {
		if (clutter_event_get_button(event) == 1) {
			reinterpret_cast< Browser * >(data)->all_clicked();
		}

		return TRUE;
	}

	gboolean Browser::movies_clicked_cb(ClutterActor *, ClutterEvent * event, gpointer data) {
		if (clutter_event_get_button(event) == 1) {
			reinterpret_cast< Browser * >(data)->movies_clicked();
		}

		return TRUE;
	}

	gboolean Browser::music_clicked_cb(ClutterActor *, ClutterEvent * event, gpointer data) {
		if (clutter_event_get_button(event) == 1) {
			reinterpret_cast< Browser * >(data)->music_clicked();
		}

		return TRUE;
	}

	gboolean Browser::scroll_dragged_cb(ClutterActor *, ClutterEvent * event, gpointer data) {
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
			gfloat y_click;
			clutter_event_get_coords(event, NULL, &y_click);

			reinterpret_cast< Browser * >(data)->scroll_dragged(y_click);
		}

		return TRUE;
	}

	void Browser::search_activated_cb(ClutterText *, gpointer data) {
		reinterpret_cast< Browser * >(data)->update_media_list();
	}

	void Browser::height_changed_cb(GObject *, GParamSpec *, gpointer data) {
		reinterpret_cast< Browser * >(data)->height_changed();
	}

	gboolean Browser::wheel_scrolled_cb(ClutterActor *, ClutterEvent * event, gpointer data) {

		switch (clutter_event_get_scroll_direction(event)) {
		case CLUTTER_SCROLL_UP:
			reinterpret_cast< Browser * >(data)->wheel_scrolled(true);
			break;
		case CLUTTER_SCROLL_DOWN:
			reinterpret_cast< Browser * >(data)->wheel_scrolled(false);
			break;
		default:
			;
		}

		return TRUE;
	}

	Browser::Browser(toolkit::InterfacePrivate * interface_private)
		: p(interface_private), type_(toolkit::Library::Type::All) {
		ClutterLayoutManager * main_layout = clutter_box_layout_new();
		clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(main_layout), 30u);
		clutter_box_layout_set_vertical(CLUTTER_BOX_LAYOUT(main_layout), TRUE);
		actor_ = clutter_box_new(main_layout);
		clutter_actor_add_constraint(actor_,
				clutter_bind_constraint_new(clutter_stage_get_default(), CLUTTER_BIND_WIDTH, -80.0f));

		ClutterLayoutManager * buttons_layout = clutter_box_layout_new();
		ClutterActor * buttons = clutter_box_new(buttons_layout);
		clutter_actor_add_constraint(buttons,
				clutter_align_constraint_new(clutter_stage_get_default(), CLUTTER_ALIGN_X_AXIS, 0.5f));
		clutter_actor_add_constraint(buttons,
				clutter_bind_constraint_new(clutter_stage_get_default(), CLUTTER_BIND_Y, 40.0f));

		all_ = clutter_cairo_texture_new(80, 20);
		draw_all_button();
		clutter_actor_set_reactive(all_, TRUE);
		g_signal_connect(all_, "button-press-event", G_CALLBACK(all_clicked_cb), this);
		g_signal_connect(all_, "enter-event", G_CALLBACK(actor_highlight_on_cb), all_);
		g_signal_connect(all_, "leave-event", G_CALLBACK(actor_highlight_off_cb), all_);
		clutter_box_pack(CLUTTER_BOX(buttons), all_, NULL, NULL);

		music_ = clutter_cairo_texture_new(80, 20);
		draw_music_button();
		clutter_actor_set_reactive(music_, TRUE);
		g_signal_connect(music_, "button-press-event", G_CALLBACK(music_clicked_cb), this);
		g_signal_connect(music_, "enter-event", G_CALLBACK(actor_highlight_on_cb), music_);
		g_signal_connect(music_, "leave-event", G_CALLBACK(actor_highlight_off_cb), music_);
		clutter_box_pack(CLUTTER_BOX(buttons), music_, NULL, NULL);

		movies_ = clutter_cairo_texture_new(80, 20);
		draw_movies_button();
		clutter_actor_set_reactive(movies_, TRUE);
		g_signal_connect(movies_, "button-press-event", G_CALLBACK(movies_clicked_cb), this);
		g_signal_connect(movies_, "enter-event", G_CALLBACK(actor_highlight_on_cb), movies_);
		g_signal_connect(movies_, "leave-event", G_CALLBACK(actor_highlight_off_cb), movies_);
		clutter_box_pack(CLUTTER_BOX(buttons), movies_, NULL, NULL);

		ClutterActor * spacing_1 = clutter_rectangle_new();
		clutter_actor_set_width(spacing_1, 40.0f);
		clutter_actor_set_opacity(spacing_1, 0);
		clutter_box_pack(CLUTTER_BOX(buttons), spacing_1, NULL, NULL);

		ClutterLayoutManager * search_layout = clutter_bin_layout_new(CLUTTER_BIN_ALIGNMENT_FIXED,
				CLUTTER_BIN_ALIGNMENT_CENTER);
		ClutterActor * search = clutter_box_new(search_layout);
		clutter_actor_set_width(search, 300.0f);

		ClutterActor * search_bg = clutter_cairo_texture_new(300, 20);

		cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(search_bg));

		cairo_move_to(context, 5.0, 0.0);
		cairo_line_to(context, 295.0, 0.0);
		cairo_curve_to(context, 299.0, 0.0, 300.0, 1.0, 300.0, 5.0);
		cairo_line_to(context, 300.0, 15.0);
		cairo_curve_to(context, 300.0, 19.0, 299.0, 20.0, 295.0, 20.0);
		cairo_line_to(context, 5.0, 20.0);
		cairo_curve_to(context, 1.0, 20.0, 0.0, 19.0, 0.0, 15.0);
		cairo_line_to(context, 0.0, 5.0);
		cairo_curve_to(context, 0.0, 1.0, 1.0, 0.0, 5.0, 0.0);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_fill_preserve(context);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke(context);

		cairo_destroy(context);

		clutter_box_pack(CLUTTER_BOX(search), search_bg, NULL, NULL);

		ClutterActor * search_icon = clutter_cairo_texture_new(18, 18);

		context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(search_icon));

		cairo_arc(context, 10.0, 8.0, 4.0, 0.0, 2.0 * M_PI);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_move_to(context, 3.0, 15.0);
		cairo_line_to(context, 7.0, 11.0);
		cairo_set_line_width(context, 3.0);
		cairo_stroke(context);

		cairo_destroy(context);

		clutter_box_pack(CLUTTER_BOX(search), search_icon, NULL, NULL);

		search_text_ = clutter_text_new();
		clutter_text_set_font_name(CLUTTER_TEXT(search_text_), "Sans 12px");
		clutter_text_set_editable(CLUTTER_TEXT(search_text_), TRUE);
		clutter_text_set_selectable(CLUTTER_TEXT(search_text_), TRUE);
		clutter_text_set_single_line_mode(CLUTTER_TEXT(search_text_), TRUE);
		clutter_actor_set_x(search_text_, 19.0f);
		clutter_actor_set_width(search_text_, 280.0f);
		clutter_actor_set_reactive(search_text_, TRUE);
		g_signal_connect(search_text_, "activate", G_CALLBACK(search_activated_cb), this);
		clutter_box_pack(CLUTTER_BOX(search), search_text_, NULL, NULL);

		clutter_box_pack(CLUTTER_BOX(buttons), search, NULL, NULL);

		ClutterActor * spacing_2 = clutter_rectangle_new();
		clutter_actor_set_width(spacing_2, 40.0f);
		clutter_actor_set_opacity(spacing_2, 0);
		clutter_box_pack(CLUTTER_BOX(buttons), spacing_2, NULL, NULL);

		add_ = clutter_cairo_texture_new(80, 20);

		context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(add_));

		cairo_move_to(context, 5.0, 0.0);
		cairo_line_to(context, 75.0, 0.0);
		cairo_curve_to(context, 79.0, 0.0, 80.0, 1.0, 80.0, 5.0);
		cairo_line_to(context, 80.0, 15.0);
		cairo_curve_to(context, 80.0, 19.0, 79.0, 20.0, 75.0, 20.0);
		cairo_line_to(context, 5.0, 20.0);
		cairo_curve_to(context, 1.0, 20.0, 0.0, 19.0, 0.0, 15.0);
		cairo_line_to(context, 0.0, 5.0);
		cairo_curve_to(context, 0.0, 1.0, 1.0, 0.0, 5.0, 0.0);
		cairo_set_source_rgb(context, 0.8, 0.8, 0.8);
		cairo_fill_preserve(context);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke(context);

		cairo_select_font_face(context, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(context, 12.0);

		cairo_text_extents_t extents;
		cairo_text_extents(context, "Add", &extents);
		cairo_move_to(context, 40.0 - ((extents.width / 2.0) + extents.x_bearing),
				10.0 - ((extents.height / 2.0) + extents.y_bearing));

		cairo_text_path(context, "Add");

		cairo_set_source_rgb(context, 0.3, 0.3, 0.3);
		cairo_fill(context);

		cairo_destroy(context);

		clutter_actor_set_reactive(add_, TRUE);
		g_signal_connect(add_, "button-press-event", G_CALLBACK(add_clicked_cb), this);
		g_signal_connect(add_, "enter-event", G_CALLBACK(actor_highlight_on_cb), add_);
		g_signal_connect(add_, "leave-event", G_CALLBACK(actor_highlight_off_cb), add_);
		clutter_box_pack(CLUTTER_BOX(buttons), add_, NULL, NULL);

		clutter_box_pack(CLUTTER_BOX(actor_), buttons, NULL, NULL);

		ClutterLayoutManager * media_browser_layout = clutter_box_layout_new();
		clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(media_browser_layout), 20u);
		ClutterActor * media_browser = clutter_box_new(media_browser_layout);
		// Clip the list at the top, Clutter fails to render anything when the sizes are bigger
		clutter_actor_set_clip(media_browser, 0.0f, 60.0f, 1e9f, 1e9f);
		clutter_actor_set_reactive(media_browser, TRUE);
		g_signal_connect(media_browser, "scroll-event", G_CALLBACK(wheel_scrolled_cb), this);
		clutter_actor_add_constraint(media_browser,
				clutter_align_constraint_new(clutter_stage_get_default(), CLUTTER_ALIGN_X_AXIS, 0.5f));

		ClutterLayoutManager * media_list_layout = clutter_flow_layout_new(CLUTTER_FLOW_HORIZONTAL);
		clutter_flow_layout_set_column_spacing(CLUTTER_FLOW_LAYOUT(media_list_layout), 15.0f);
		clutter_flow_layout_set_homogeneous(CLUTTER_FLOW_LAYOUT(media_list_layout), TRUE);
		clutter_flow_layout_set_row_spacing(CLUTTER_FLOW_LAYOUT(media_list_layout), 15.0f);
		media_list_ = clutter_box_new(media_list_layout);
		clutter_box_pack(CLUTTER_BOX(media_browser), media_list_, NULL, NULL);

		clutter_actor_add_constraint(media_list_,
				clutter_snap_constraint_new(actor_, CLUTTER_SNAP_EDGE_TOP, CLUTTER_SNAP_EDGE_TOP, 70.0f));

		ClutterLayoutManager * scroll_layout = clutter_bin_layout_new(CLUTTER_BIN_ALIGNMENT_CENTER,
				CLUTTER_BIN_ALIGNMENT_FIXED);
		ClutterActor * scroll = clutter_box_new(scroll_layout);
		clutter_actor_add_constraint(scroll,
				clutter_align_constraint_new(clutter_stage_get_default(), CLUTTER_ALIGN_Y_AXIS, 0.5f));

		scroll_hidden_ = clutter_rectangle_new();
		clutter_actor_set_width(scroll_hidden_, 18.0f);
		clutter_actor_set_opacity(scroll_hidden_, 0);
		clutter_actor_set_reactive(scroll_hidden_, TRUE);
		g_signal_connect(scroll_hidden_, "button-press-event", G_CALLBACK(scroll_dragged_cb), this);
		g_signal_connect(scroll_hidden_, "motion-event", G_CALLBACK(scroll_dragged_cb), this);
		clutter_box_pack(CLUTTER_BOX(scroll), scroll_hidden_, NULL, NULL);

		ClutterColor black = {0, 0, 0, 255};
		ClutterColor white = {255, 255, 255, 255};
		scroll_line_ = clutter_rectangle_new_with_color(&white);
		clutter_rectangle_set_border_color(CLUTTER_RECTANGLE(scroll_line_), &black);
		clutter_rectangle_set_border_width(CLUTTER_RECTANGLE(scroll_line_), 1);
		clutter_actor_set_width(scroll_line_, 4.0f);
		clutter_box_pack(CLUTTER_BOX(scroll), scroll_line_, NULL, NULL);

		scroll_handle_ = clutter_cairo_texture_new(10, 12);

		context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(scroll_handle_));

		cairo_rectangle(context, 1.0, 1.0, 8.0, 10.0);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_fill_preserve(context);
		cairo_set_line_width(context, 1.0);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke(context);

		cairo_destroy(context);

		g_signal_connect(scroll_hidden_, "enter-event", G_CALLBACK(actor_highlight_on_cb), scroll_handle_);
		g_signal_connect(scroll_hidden_, "leave-event", G_CALLBACK(actor_highlight_off_cb), scroll_handle_);
		clutter_box_pack(CLUTTER_BOX(scroll), scroll_handle_, NULL, NULL);

		clutter_box_pack(CLUTTER_BOX(media_browser), scroll, NULL, NULL);

		clutter_box_pack(CLUTTER_BOX(actor_), media_browser, NULL, NULL);

		g_signal_connect(clutter_stage_get_default(), "notify::height", G_CALLBACK(height_changed_cb), this);

		update_media_list();
	}

/*
	Called whenever the add button is clicked
*/
	void Browser::add_clicked() {
		p->add();
	}

/*
	Called whenever the display all media button is clicked
*/
	void Browser::all_clicked() {
		switch (type_) {
		case toolkit::Library::Type::Movies:
			type_ = toolkit::Library::Type::All;
			draw_movies_button();
			draw_all_button();
			update_media_list();
			break;
		case toolkit::Library::Type::Music:
			type_ = toolkit::Library::Type::All;
			draw_music_button();
			draw_all_button();
			update_media_list();
			break;
		default:
			;
		}

	}

/*
	Clear the list of media items
*/
	void Browser::clear_media_list() {
		GList * children = clutter_container_get_children(CLUTTER_CONTAINER(media_list_));
		while (children) {
			ClutterActor * child = static_cast< ClutterActor * >(children->data);
			children = g_list_next(children);

			clutter_container_remove_actor(CLUTTER_CONTAINER(media_list_), child);
		}

		item_list_.clear();
	}

/*
	Draws the display all media button
*/
	void Browser::draw_all_button() {
		clutter_cairo_texture_clear(CLUTTER_CAIRO_TEXTURE(all_));
		cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(all_));

		cairo_move_to(context, 80.0, 0.0);
		cairo_line_to(context, 5.0, 0.0);
		cairo_curve_to(context, 1.0, 0.0, 0.0, 1.0, 0.0, 5.0);
		cairo_line_to(context, 0.0, 15.0);
		cairo_curve_to(context, 0.0, 19.0, 1.0, 20.0, 5.0, 20.0);
		cairo_line_to(context, 80.0, 20.0);

		if (type_ == toolkit::Library::Type::All) {
			cairo_set_source_rgb(context, 0.2, 0.2, 0.2);
		} else {
			cairo_set_source_rgb(context, 0.8, 0.8, 0.8);
		}
		cairo_fill_preserve(context);

		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke(context);

		cairo_select_font_face(context, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(context, 12.0);

		cairo_text_extents_t extents;
		cairo_text_extents(context, "All", &extents);
		cairo_move_to(context, 40.0 - ((extents.width / 2.0) + extents.x_bearing),
				10.0 - ((extents.height / 2.0) + extents.y_bearing));

		cairo_text_path(context, "All");

		if (type_ == toolkit::Library::Type::All) {
			cairo_set_source_rgb(context, 0.9, 0.9, 0.9);
		} else {
			cairo_set_source_rgb(context, 0.3, 0.3, 0.3);
		}
		cairo_fill(context);

		cairo_destroy(context);
	}

/*
	Draws the display movies button
*/
	void Browser::draw_movies_button() {
		clutter_cairo_texture_clear(CLUTTER_CAIRO_TEXTURE(movies_));
		cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(movies_));

		cairo_move_to(context, 0.0, 0.0);
		cairo_line_to(context, 75.0, 0.0);
		cairo_curve_to(context, 79.0, 0.0, 80.0, 1.0, 80.0, 5.0);
		cairo_line_to(context, 80.0, 15.0);
		cairo_curve_to(context, 80.0, 19.0, 79.0, 20.0, 75.0, 20.0);
		cairo_line_to(context, 0.0, 20.0);

		if (type_ == toolkit::Library::Type::Movies) {
			cairo_set_source_rgb(context, 0.2, 0.2, 0.2);
		} else {
			cairo_set_source_rgb(context, 0.8, 0.8, 0.8);
		}
		cairo_fill_preserve(context);

		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke(context);

		cairo_select_font_face(context, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(context, 12.0);

		cairo_text_extents_t extents;
		cairo_text_extents(context, "Movies", &extents);
		cairo_move_to(context, 40.0 - ((extents.width / 2.0) + extents.x_bearing),
				10.0 - ((extents.height / 2.0) + extents.y_bearing));

		cairo_text_path(context, "Movies");

		if (type_ == toolkit::Library::Type::Movies) {
			cairo_set_source_rgb(context, 0.9, 0.9, 0.9);
		} else {
			cairo_set_source_rgb(context, 0.3, 0.3, 0.3);
		}
		cairo_fill(context);

		cairo_destroy(context);
	}

/*
	Draws the display music button
*/
	void Browser::draw_music_button() {
		clutter_cairo_texture_clear(CLUTTER_CAIRO_TEXTURE(music_));
		cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(music_));

		cairo_rectangle(context, 0.0, 0.0, 80.0, 20.0);

		if (type_ == toolkit::Library::Type::Music) {
			cairo_set_source_rgb(context, 0.2, 0.2, 0.2);
		} else {
			cairo_set_source_rgb(context, 0.8, 0.8, 0.8);
		}
		cairo_fill_preserve(context);

		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke(context);

		cairo_select_font_face(context, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(context, 12.0);

		cairo_text_extents_t extents;
		cairo_text_extents(context, "Music", &extents);
		cairo_move_to(context, 40.0 - ((extents.width / 2.0) + extents.x_bearing),
				10.0 - ((extents.height / 2.0) + extents.y_bearing));

		cairo_text_path(context, "Music");

		if (type_ == toolkit::Library::Type::Music) {
			cairo_set_source_rgb(context, 0.9, 0.9, 0.9);
		} else {
			cairo_set_source_rgb(context, 0.3, 0.3, 0.3);
		}
		cairo_fill(context);

		cairo_destroy(context);
	}

/*
	Called whenever the stage's height changes
*/
	void Browser::height_changed() {
		clutter_actor_set_height(scroll_line_, clutter_actor_get_height(clutter_stage_get_default()) * 0.6f);
		clutter_actor_set_height(scroll_hidden_, clutter_actor_get_height(clutter_stage_get_default()) * 0.6f);
	}

/*
	Called whenever the display movies button is clicked
*/
	void Browser::movies_clicked() {
		switch (type_) {
		case toolkit::Library::Type::All:
			type_ = toolkit::Library::Type::Movies;
			draw_all_button();
			draw_movies_button();
			update_media_list();
			break;
		case toolkit::Library::Type::Music:
			type_ = toolkit::Library::Type::Movies;
			draw_music_button();
			draw_movies_button();
			update_media_list();
			break;
		default:
			;
		}

		update_media_list();
	}

/*
	Called whenever the display music button is clicked
*/
	void Browser::music_clicked() {
		switch (type_) {
		case toolkit::Library::Type::All:
			type_ = toolkit::Library::Type::Music;
			draw_all_button();
			draw_music_button();
			update_media_list();
			break;
		case toolkit::Library::Type::Movies:
			type_ = toolkit::Library::Type::Music;
			draw_movies_button();
			draw_music_button();
			update_media_list();
			break;
		default:
			;
		}

		update_media_list();
	}

/*
	Called whenever the scroll handle is dragged
*/
	void Browser::scroll_dragged(float y) {
		if (item_list_.size() == 0) {
			return;
		}

		float columns = std::floor(clutter_actor_get_width(media_list_) / 195.0f);
		gfloat height = std::ceil(item_list_.size() / columns) *
				clutter_actor_get_height(item_list_.front().actor());

		if (height < (clutter_actor_get_height(clutter_stage_get_default()) - 100.0f)) {
			// List is completely visible
			return;
		}

		gfloat y_line;
		clutter_actor_get_transformed_position(scroll_line_, NULL, &y_line);
		gfloat line_height = clutter_actor_get_height(scroll_line_);

		gfloat offset = (((y - y_line) * height) / line_height);

		clutter_actor_move_anchor_point(media_list_, 0.0f, offset);

		gfloat handle_y = y - y_line - clutter_actor_get_height(scroll_handle_) + 2.0f;
		clutter_actor_set_y(scroll_handle_, handle_y > 0.0f ? handle_y : 0.0f);
	}

/*
	Update the displayed list of media items
*/
	void Browser::update_media_list() {
		clear_media_list();

		std::vector< toolkit::MediaItem > list;
		if (*(clutter_text_get_text(CLUTTER_TEXT(search_text_))) == '\0') {
			// Search box is empty
			list = library_.list(type_);
		} else {
			// Search text exists
			list = library_.search(type_, clutter_text_get_text(CLUTTER_TEXT(search_text_)));
		}
		item_list_.reserve(list.size());

		for (std::vector< toolkit::MediaItem >::const_iterator i = list.begin(); i != list.end(); ++i) {
			item_list_.emplace_back(*i, p);
			clutter_box_pack(CLUTTER_BOX(media_list_), item_list_.back().actor(), NULL, NULL);
		}

		// Scroll to top of list
		clutter_actor_move_anchor_point(media_list_, 0.0f, 0.0f);
		clutter_actor_set_y(scroll_handle_, 0.0f);
	}

/*
	Called whenever the mouse wheel is scrolled
*/
	void Browser::wheel_scrolled(bool up) {
		if (item_list_.size() == 0) {
			return;
		}

		float columns = std::floor(clutter_actor_get_width(media_list_) / 195.0f);
		gfloat height = std::ceil(item_list_.size() / columns) *
				clutter_actor_get_height(item_list_.front().actor());

		if (height < (clutter_actor_get_height(clutter_stage_get_default()) - 100.0f)) {
			// List is completely visible
			return;
		}

		gfloat offset;
		clutter_actor_get_anchor_point(media_list_, NULL, &offset);

		if (up) {
			offset -= 10.0f;
		} else {
			offset += 10.0f;
		}

		if (offset < 0.0f) {
			offset = 0.0f;
		} else if (offset > height) {
			offset = height;
		}

		clutter_actor_set_anchor_point(media_list_, 0.0f, offset);

		gfloat handle_y = ((offset / height) * clutter_actor_get_height(scroll_line_)) -
				clutter_actor_get_height(scroll_handle_);
		clutter_actor_set_y(scroll_handle_, handle_y > 0.0f ? handle_y : 0.0f);
	}

/*
	List of media items needs updating
*/
	void Browser::update() {
		update_media_list();
	}
}
