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

namespace { namespace clutter {
	class PlayArea {
		ClutterActor * actor_;
		ClutterMedia * media_;

	public:
		PlayArea(ClutterStage * stage) {
			ClutterLayoutManager * layout = clutter_box_layout_new();
			actor_ = clutter_box_new(layout);
			clutter_actor_add_constraint(actor_, clutter_bind_constraint_new(CLUTTER_ACTOR(stage), CLUTTER_BIND_SIZE, 0.0));

			// Video play area
			media_ = CLUTTER_MEDIA(clutter_gst_video_texture_new());
			clutter_texture_set_keep_aspect_ratio(CLUTTER_TEXTURE(media_), TRUE);
			clutter_box_pack(CLUTTER_BOX(actor_), CLUTTER_ACTOR(media_), "expand", TRUE, NULL);

			clutter_container_add_actor(CLUTTER_CONTAINER(stage), actor_);
		}

		ClutterActor * actor() const {
			return actor_;
		}

		ClutterMedia * media() const {
			return media_;
		}
	};
}}
