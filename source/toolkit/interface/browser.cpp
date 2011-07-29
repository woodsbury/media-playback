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

#include <utility>

#include "browser.hpp"
#include "interface_private.hpp"

namespace interface {
	BrowserItem::BrowserItem(toolkit::MediaItem media_item)
		: item_(std::move(media_item)), actor_(nullptr), list_(nullptr) {}

	BrowserItem::~BrowserItem() {
		removeFromList();
	}

/*
	Add an actor representing this item to the given container
*/
	void BrowserItem::addToList(ClutterContainer * list) {
		;
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
				clutter_align_constraint_new(clutter_stage_get_default(), CLUTTER_ALIGN_X_AXIS, 0.5f));
		clutter_actor_add_constraint(actor_,
				clutter_align_constraint_new(clutter_stage_get_default(), CLUTTER_ALIGN_Y_AXIS, 0.5f));

		ClutterLayoutManager * media_list_layout = clutter_flow_layout_new(CLUTTER_FLOW_HORIZONTAL);
		clutter_flow_layout_set_homogeneous(CLUTTER_FLOW_LAYOUT(media_list_layout), TRUE);
		clutter_flow_layout_set_column_spacing(CLUTTER_FLOW_LAYOUT(media_list_layout), 10.0f);
		clutter_flow_layout_set_row_spacing(CLUTTER_FLOW_LAYOUT(media_list_layout), 10.0f);
		media_list_ = clutter_box_new(media_list_layout);
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
			item_list_.emplace_back(*i);
		}
	}
}
