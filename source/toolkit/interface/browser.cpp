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

#include "browser.hpp"
#include "interface_private.hpp"

namespace interface {
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

	void Browser::clear_media_list() {
		GList * item = clutter_container_get_children(CLUTTER_CONTAINER(media_list_));

		while (item != NULL) {
			clutter_container_remove_actor(CLUTTER_CONTAINER(media_list_), CLUTTER_ACTOR(item));
			item = g_list_next(item);
		}

		g_list_free(g_list_first(item));
	}

	void Browser::update_media_list() {
		clear_media_list();

		std::vector< toolkit::MediaItem > list(library_.list(toolkit::Library::Type::All));
	}
}
