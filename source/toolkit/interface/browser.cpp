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
		g_signal_connect(actor_, "button-press-event", G_CALLBACK(item_clicked_cb), this);

		thumbnail_ = clutter_cairo_texture_new(180, 180);
		draw_thumbnail();
		g_signal_connect(actor_, "enter-event", G_CALLBACK(Actor::actor_scale_on_cb),
				thumbnail_);
		g_signal_connect(actor_, "leave-event", G_CALLBACK(Actor::actor_scale_off_cb),
				thumbnail_);
		clutter_box_pack(CLUTTER_BOX(actor_), thumbnail_, NULL, NULL);

		ClutterColor white = {255, 255, 255, 255};
		ClutterActor * title = clutter_text_new_full("Sans", item_.title().c_str(), &white);
		g_signal_connect(actor_, "enter-event", G_CALLBACK(Actor::actor_highlight_on_cb),
				title);
		g_signal_connect(actor_, "leave-event", G_CALLBACK(Actor::actor_highlight_off_cb),
				title);
		clutter_box_pack(CLUTTER_BOX(actor_), title, NULL, NULL);

		g_object_ref_sink(actor_);
	}

	BrowserItem::~BrowserItem() {
		g_object_unref(actor_);
	}

/*
	Renders the thumnail for the item
*/
	void BrowserItem::draw_thumbnail() {
		cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(thumbnail_));

		if (!item_.thumbnail_file().empty() && core::Path::exists(item_.thumbnail_file())) {
			cairo_surface_t * image = cairo_image_surface_create_from_png(item_.thumbnail_file().c_str());

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

	void Browser::height_changed_cb(GObject *, GParamSpec *, gpointer data) {
		reinterpret_cast< Browser * >(data)->height_changed();
	}

	Browser::Browser(toolkit::InterfacePrivate * interface_private)
		: p(interface_private) {
		ClutterLayoutManager * main_layout = clutter_box_layout_new();
		clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(main_layout), 30u);
		clutter_box_layout_set_vertical(CLUTTER_BOX_LAYOUT(main_layout), TRUE);
		actor_ = clutter_box_new(main_layout);
		clutter_actor_add_constraint(actor_,
				clutter_bind_constraint_new(clutter_stage_get_default(), CLUTTER_BIND_WIDTH, -80.0f));

		ClutterLayoutManager * media_browser_layout = clutter_box_layout_new();
		clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(media_browser_layout), 20u);
		ClutterActor * media_browser = clutter_box_new(media_browser_layout);
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

		cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(scroll_handle_));

		cairo_rectangle(context, 1.0, 1.0, 8.0, 10.0);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_fill_preserve(context);
		cairo_set_line_width(context, 1.0);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke(context);

		cairo_destroy(context);

		g_signal_connect(scroll_hidden_, "enter-event", G_CALLBACK(actor_highlight_on_cb),
				scroll_handle_);
		g_signal_connect(scroll_hidden_, "leave-event", G_CALLBACK(actor_highlight_off_cb),
				scroll_handle_);
		clutter_box_pack(CLUTTER_BOX(scroll), scroll_handle_, NULL, NULL);

		clutter_box_pack(CLUTTER_BOX(media_browser), scroll, NULL, NULL);

		clutter_box_pack(CLUTTER_BOX(actor_), media_browser, NULL, NULL);

		g_signal_connect(clutter_stage_get_default(), "notify::height", G_CALLBACK(height_changed_cb), this);

		update_media_list();
	}

/*
	Clear the list of media items
*/
	void Browser::clear_media_list() {
		item_list_.clear();
	}

/*
	Called whenever the scroll handle is dragged
*/
	void Browser::scroll_dragged(float y) {
		float columns = std::floor(clutter_actor_get_width(media_list_) / 195.0f);
		gfloat height = (item_list_.size() / columns) * 210.0f;

		if (height < clutter_actor_get_height(clutter_stage_get_default())) {
			// List is completely visible
			return;
		}


		gfloat y_line;
		clutter_actor_get_transformed_position(scroll_line_, NULL, &y_line);
		gfloat line_height = clutter_actor_get_height(scroll_line_);

		gfloat offset = (((y - y_line) * height) / line_height);

		clutter_actor_move_anchor_point(media_list_, 0.0f, offset);

		gfloat handle_y = y - y_line - clutter_actor_get_height(scroll_handle_) + 2.0f;
		clutter_actor_set_y(scroll_handle_, handle_y > 0 ? handle_y : 0.0f);
	}

/*
	Update the displayed list of media items
*/
	void Browser::update_media_list() {
		clear_media_list();

		std::vector< toolkit::MediaItem > list(library_.list(toolkit::Library::Type::All));

		for (std::vector< toolkit::MediaItem >::const_iterator i = list.begin(); i != list.end(); ++i) {
			item_list_.emplace_back(*i, p);
			clutter_box_pack(CLUTTER_BOX(media_list_), item_list_.back().actor(), NULL, NULL);
		}
	}

/*
	Called whenever the stage's height changes
*/
	void Browser::height_changed() {
		clutter_actor_set_height(scroll_line_, clutter_actor_get_height(clutter_stage_get_default()) * 0.6f);
		clutter_actor_set_height(scroll_hidden_, clutter_actor_get_height(clutter_stage_get_default()) * 0.6f);
	}
}
