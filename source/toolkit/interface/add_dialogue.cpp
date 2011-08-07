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

extern "C" {
#include <cairo.h>
}

#include "add_dialogue.hpp"
#include "interface_private.hpp"

namespace interface {
	AddDialogue::AddDialogue(toolkit::InterfacePrivate * interface_private)
		: p(interface_private){
		ClutterLayoutManager * main_layout = clutter_bin_layout_new(CLUTTER_BIN_ALIGNMENT_FIXED,
				CLUTTER_BIN_ALIGNMENT_FIXED);
		actor_ = clutter_box_new(main_layout);

		ClutterColor black = {0, 0, 0, 150};
		ClutterActor * background  = clutter_rectangle_new();
		clutter_rectangle_set_color(CLUTTER_RECTANGLE(background), &black);
		clutter_actor_add_constraint(background,
				clutter_bind_constraint_new(clutter_stage_get_default(), CLUTTER_BIND_HEIGHT, 0.0f));
		clutter_actor_add_constraint(background,
				clutter_bind_constraint_new(clutter_stage_get_default(), CLUTTER_BIND_WIDTH, 0.0f));
		clutter_box_pack(CLUTTER_BOX(actor_), background, NULL, NULL);

		ClutterColor grey = {200, 200, 200, 255};
		ClutterActor * dialogue = clutter_rectangle_new();
		clutter_rectangle_set_color(CLUTTER_RECTANGLE(dialogue), &grey);
		clutter_actor_add_constraint(dialogue,
				clutter_bind_constraint_new(background, CLUTTER_BIND_HEIGHT, -150.0f));
		clutter_actor_add_constraint(dialogue,
				clutter_bind_constraint_new(background, CLUTTER_BIND_WIDTH, -150.0f));
		clutter_actor_add_constraint(dialogue,
				clutter_align_constraint_new(clutter_stage_get_default(), CLUTTER_ALIGN_X_AXIS, 0.5f));
		clutter_actor_add_constraint(dialogue,
				clutter_align_constraint_new(clutter_stage_get_default(), CLUTTER_ALIGN_Y_AXIS, 0.5f));
		clutter_box_pack(CLUTTER_BOX(actor_), dialogue, NULL, NULL);

		ClutterActor * close_button = clutter_cairo_texture_new(15, 15);

		cairo_t * context = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(close_button));

		cairo_move_to(context, 1.0, 3.0);
		cairo_line_to(context, 3.0, 1.0);
		cairo_line_to(context, 8.0, 6.0);
		cairo_line_to(context, 13.0, 1.0);
		cairo_line_to(context, 15.0, 3.0);
		cairo_line_to(context, 10.0, 8.0);
		cairo_line_to(context, 15.0, 13.0);
		cairo_line_to(context, 13.0, 15.0);
		cairo_line_to(context, 8.0, 10.0);
		cairo_line_to(context, 3.0, 15.0);
		cairo_line_to(context, 1.0, 13.0);
		cairo_line_to(context, 6.0, 8.0);
		cairo_close_path(context);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_fill(context);

		cairo_destroy(context);

		clutter_box_pack(CLUTTER_BOX(actor_), close_button, NULL, NULL);
	}
}
