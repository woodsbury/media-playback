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
			return FALSE;
		}

/*
		Highlights the actor
*/
		static gboolean actor_highlight_on_cb(ClutterActor *, ClutterEvent *, gpointer data) {
			if (clutter_feature_available(CLUTTER_FEATURE_SHADERS_GLSL)) {
				ClutterColor highlight = {250, 125, 0, 200};
				clutter_actor_add_effect(reinterpret_cast< ClutterActor * >(data), clutter_colorize_effect_new(&highlight));
			}

			return FALSE;
		}

/*
		Scales the actor
*/
		static gboolean actor_scale_on_cb(ClutterActor *, ClutterEvent *, gpointer data) {
			clutter_actor_set_scale_with_gravity(CLUTTER_ACTOR(data), 1.1, 1.1, CLUTTER_GRAVITY_CENTER);
			return FALSE;
		}

/*
		Removes scaling from an actor
*/
		static gboolean actor_scale_off_cb(ClutterActor *, ClutterEvent *, gpointer data) {
			clutter_actor_set_scale(CLUTTER_ACTOR(data), 1.0, 1.0);
			return FALSE;
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
