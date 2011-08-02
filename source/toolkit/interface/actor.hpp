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

#ifndef _INTERFACE_ACTOR_HPP
#define _INTERFACE_ACTOR_HPP

extern "C" {
#include <clutter/clutter.h>
}

namespace interface {
	class Actor {
	protected:
		ClutterActor * actor_;

	public:
/*
		Removes a highlight from the actor
*/
		static gboolean actor_highlight_off_cb(ClutterActor *, ClutterEvent *, gpointer data) {
			clutter_actor_clear_effects(reinterpret_cast< ClutterActor * >(data));
			return TRUE;
		}

/*
		Highlights the actor
*/
		static gboolean actor_highlight_on_cb(ClutterActor *, ClutterEvent *, gpointer data) {
			if (clutter_feature_available(CLUTTER_FEATURE_SHADERS_GLSL)) {
				ClutterColor highlight = {0, 230, 100, 255};
				clutter_actor_add_effect(reinterpret_cast< ClutterActor * >(data), clutter_colorize_effect_new(&highlight));
			}

			return TRUE;
		}

/*
		Hides the actor after an animation
*/
		static void hide_after_cb(ClutterAnimation *, ClutterActor * actor) {
			clutter_actor_hide_all(actor);
		}

		ClutterActor * actor() {
			return actor_;
		}
	};
}

#endif
