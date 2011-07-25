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
	}
}
