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
	class MediaButtons {
		ClutterActor * actor_;

		ClutterActor * play_;
		ClutterActor * line_;
		ClutterActor * handle_;

		ClutterMedia * media_;

	static gboolean play_clicked(ClutterActor *, ClutterEvent * event, gpointer data) {
		if (clutter_event_get_button(event) == 1) {
			// Left mouse button pressed
			dprint("Play clicked");
			ClutterMedia * media = reinterpret_cast< clutter::MediaButtons * >(data)->media();
			gchar * uri = clutter_media_get_uri(media);

			if (uri != NULL) {
				gboolean playing = !clutter_media_get_playing(media);
				clutter_media_set_playing(media, playing);
				reinterpret_cast< clutter::MediaButtons * >(data)->setPlaying(playing);
				g_free(uri);
			} else {
				dprint("No URI set");
				reinterpret_cast< clutter::MediaButtons * >(data)->setPlaying(false);
			}
		}

		return TRUE;
	}


	static gboolean update_seek(gpointer data) {
		reinterpret_cast< clutter::MediaButtons * >(data)->updateSeek();
		return TRUE;
	}

	public:
		MediaButtons(ClutterStage * stage, ClutterMedia * media)
			: media_(media) {
			ClutterColor white = {250, 250, 250, 200};
			ClutterColor black = {5, 5, 5, 200};

			ClutterLayoutManager * layout = clutter_box_layout_new();
			clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(layout), 7);
			actor_ = clutter_box_new(layout);
			clutter_actor_add_constraint(actor_, clutter_align_constraint_new(CLUTTER_ACTOR(stage),
					CLUTTER_ALIGN_X_AXIS, 0.5));
			clutter_actor_add_constraint(actor_, clutter_align_constraint_new(CLUTTER_ACTOR(stage),
					CLUTTER_ALIGN_Y_AXIS, 0.99));

			// A play button actor
			play_ = clutter_text_new_full("Sans 14px", "Play", &white);
			clutter_actor_set_reactive(play_, TRUE);
			g_signal_connect(play_, "button-press-event", G_CALLBACK(play_clicked), this);
			clutter_box_pack(CLUTTER_BOX(actor_), play_, NULL, NULL);

			// Seek widget actor
			ClutterLayoutManager * seek_layout = clutter_bin_layout_new(CLUTTER_BIN_ALIGNMENT_FIXED,
					CLUTTER_BIN_ALIGNMENT_CENTER);
			ClutterActor * seek_box = clutter_box_new(seek_layout);

			// A seek line
			line_ = clutter_rectangle_new_with_color(&white);
			clutter_rectangle_set_border_color(CLUTTER_RECTANGLE(line_), &black);
			clutter_rectangle_set_border_width(CLUTTER_RECTANGLE(line_), 1);
			clutter_actor_set_size(line_, 400.0f, 3.0f);
			clutter_box_pack(CLUTTER_BOX(seek_box), line_, NULL, NULL);

			// A seek handle
			handle_ = clutter_rectangle_new_with_color(&white);
			clutter_rectangle_set_border_color(CLUTTER_RECTANGLE(handle_), &black);
			clutter_rectangle_set_border_width(CLUTTER_RECTANGLE(handle_), 1);
			clutter_actor_set_size(handle_, 5.0f, 10.0f);
			clutter_box_pack(CLUTTER_BOX(seek_box), handle_, NULL, NULL);

			clutter_box_pack(CLUTTER_BOX(actor_), seek_box, NULL, NULL);

			clutter_container_add_actor(CLUTTER_CONTAINER(stage), actor_);
		}

		ClutterActor * actor() const {
			return actor_;
		}

/*
		Sets whether the media buttons should be displaying a playing state or not
*/
		void setPlaying(bool playing) {
			static guint timeout_id(0);

			if (playing) {
				clutter_text_set_text(CLUTTER_TEXT(play_), "Pause");
			} else {
				clutter_text_set_text(CLUTTER_TEXT(play_), "Play");
			}

			if (playing && (timeout_id == 0)) {
				timeout_id = g_timeout_add(500, update_seek, this);
			} else if (!playing && (timeout_id > 0)) {
				g_source_remove(timeout_id);
				timeout_id = 0;
			}
		}

/*
		Updates the seek handle position
*/
		void updateSeek(gfloat width = 0.0f) const {
			if (width != 0.0f) {
				// Update width of seek line
				clutter_actor_set_width(line_, width * 0.80f);
			}

			clutter_actor_set_x(handle_, (clutter_actor_get_width(line_) - 6.0f) * clutter_media_get_progress(media_));
		}

		ClutterMedia * media() {
			return media_;
		}
	};
}}
