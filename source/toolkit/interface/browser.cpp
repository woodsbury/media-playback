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
		: p(interface_private), item_(std::move(media_item)), actor_(nullptr), list_(nullptr) {}

	BrowserItem::~BrowserItem() {
		removeFromList();
	}

/*
	Called whenever the item is clicked
*/
	void BrowserItem::item_clicked() {
		p->play(item_.uri().c_str(), item_.title().c_str());
	}

/*
	Add an actor representing this item to the given container
*/
	void BrowserItem::addToList(ClutterContainer * list) {
		removeFromList();
		assert(actor_ == nullptr);
		assert(list_ == nullptr);

		ClutterLayoutManager * item_layout = clutter_box_layout_new();
		clutter_box_layout_set_vertical(CLUTTER_BOX_LAYOUT(item_layout), TRUE);
		actor_ = clutter_box_new(item_layout);
		clutter_actor_set_reactive(actor_, TRUE);
		g_signal_connect(actor_, "button-press-event", G_CALLBACK(item_clicked_cb), this);

		ClutterActor * thumbnail = clutter_cairo_texture_new(176, 99);

		cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(thumbnail));

		if (!item_.thumbnail_file().empty() && core::Path::exists(item_.thumbnail_file())) {
			cairo_surface_t * image = cairo_image_surface_create_from_png(item_.thumbnail_file().c_str());

			double scale_factor = std::min(176.0/cairo_image_surface_get_width(image),
					99.0/cairo_image_surface_get_height(image));
			cairo_translate(context, 88.0 - ((cairo_image_surface_get_width(image) * scale_factor) / 2.0), 0.0);
			cairo_scale(context, scale_factor, scale_factor);

			cairo_set_source_surface(context, image, 0.0, 0.0);
			cairo_paint(context);

			cairo_surface_destroy(image);
		} else {
			// Draw generic thumbnail
			cairo_rectangle(context, 1.0, 1.0, 174.0, 97.0);

			cairo_set_source_rgb(context, 0.3, 0.3, 0.4);
			cairo_fill(context);
		}

		cairo_destroy(context);

		clutter_box_pack(CLUTTER_BOX(actor_), thumbnail, NULL, NULL);

		ClutterColor white = {255, 255, 255, 255};
		ClutterActor * title = clutter_text_new_full("Sans", item_.title().c_str(), &white);
		clutter_box_pack(CLUTTER_BOX(actor_), title, NULL, NULL);

		clutter_container_add_actor(list, actor_);

		list_ = list;
	}

/*
	Remove the actor represented by this item from its container
*/
	void BrowserItem::removeFromList() {
		if ((list_ != nullptr) && (actor_ != nullptr)) {
			clutter_container_remove_actor(list_, actor_);
			list_ = nullptr;
			actor_ = nullptr;
		}
	}

	Browser::Browser(toolkit::InterfacePrivate * interface_private)
		: p(interface_private) {
		ClutterLayoutManager * main_layout = clutter_box_layout_new();
		clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(main_layout), 30u);
		clutter_box_layout_set_vertical(CLUTTER_BOX_LAYOUT(main_layout), TRUE);
		actor_ = clutter_box_new(main_layout);
		clutter_actor_add_constraint(actor_,
				clutter_align_constraint_new(clutter_stage_get_default(), CLUTTER_ALIGN_Y_AXIS, 0.5f));
		clutter_actor_add_constraint(actor_,
				clutter_bind_constraint_new(clutter_stage_get_default(), CLUTTER_BIND_WIDTH, -20.0f));

		ClutterLayoutManager * media_list_layout = clutter_flow_layout_new(CLUTTER_FLOW_HORIZONTAL);
		clutter_flow_layout_set_homogeneous(CLUTTER_FLOW_LAYOUT(media_list_layout), TRUE);
		clutter_flow_layout_set_column_spacing(CLUTTER_FLOW_LAYOUT(media_list_layout), 10.0f);
		clutter_flow_layout_set_row_spacing(CLUTTER_FLOW_LAYOUT(media_list_layout), 10.0f);
		media_list_ = clutter_box_new(media_list_layout);
		clutter_actor_add_constraint(media_list_,
				clutter_align_constraint_new(clutter_stage_get_default(), CLUTTER_ALIGN_X_AXIS, 0.5f));
		clutter_actor_add_constraint(media_list_,
				clutter_bind_constraint_new(clutter_stage_get_default(), CLUTTER_BIND_HEIGHT, -20.0f));
		clutter_box_pack(CLUTTER_BOX(actor_), media_list_, NULL, NULL);

		update_media_list();
	}

/*
	Clear the list of media items
*/
	void Browser::clear_media_list() {
		item_list_.clear();
	}

/*
	Update the displayed list of media items
*/
	void Browser::update_media_list() {
		clear_media_list();

		std::vector< toolkit::MediaItem > list(library_.list(toolkit::Library::Type::All));

		for (std::vector< toolkit::MediaItem >::const_iterator i = list.begin(); i != list.end(); ++i) {
			item_list_.emplace_back(*i, p);
			item_list_.back().addToList(CLUTTER_CONTAINER(media_list_));
		}
	}
}
