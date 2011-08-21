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

#include <debug.hpp>
#include <toolkit/inspector.hpp>

extern "C" {
#include <gst/gst.h>
}

namespace {
	namespace gstreamer {
		/*
			Inheriting this class will automatically initialiser GStreamer
		*/
		class Initialiser {
		public:
			Initialiser() {
				if (!gst_is_initialized()) {
					dprint("Initialising GStreamer");
					gst_init(NULL, NULL);

#if defined(DEBUG) && !defined(GST_DISABLE_GST_DEBUG)
					gst_debug_set_default_threshold(GST_LEVEL_ERROR);
					gst_debug_set_active(true);
#endif
				}
			}
		};
	}
}

namespace toolkit {
	class InspectorPrivate
			: gstreamer::Initialiser {
		static void bus_message_cb(GstBus * bus, GstMessage * message, gpointer data);
		static void pad_added_cb(GstElement * element, GstPad * pad, gpointer data);

		GstElement * pipeline_;

		bool has_audio_;
		bool has_video_;

		std::string title_;
		std::string album_;

		void bus_message(GstMessage * message);
		void pad_added(GstElement * element, GstPad * pad);

	public:
		InspectorPrivate(char const * uri);
		~InspectorPrivate();

		inline bool audio() const;
		inline bool video() const;

		inline std::string title() const;
		inline std::string album() const;
	};

	void InspectorPrivate::bus_message_cb(GstBus *, GstMessage * message, gpointer data) {
		reinterpret_cast< InspectorPrivate * >(data)->bus_message(message);
	}

	void InspectorPrivate::pad_added_cb(GstElement * element, GstPad * pad, gpointer data) {
		reinterpret_cast< InspectorPrivate * >(data)->pad_added(element, pad);
	}

	InspectorPrivate::InspectorPrivate(char const * uri)
		: has_audio_(false), has_video_(false) {
		pipeline_ = gst_pipeline_new(NULL);

		GstElement * decoder = gst_element_factory_make("uridecodebin", NULL);
		g_object_set(G_OBJECT(decoder), "uri", uri, NULL);
		gst_bin_add(GST_BIN(pipeline_), decoder);
		g_signal_connect(decoder, "pad-added", G_CALLBACK(pad_added_cb), this);

		GstBus * bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline_));
		gst_bus_enable_sync_message_emission(bus);
		g_signal_connect(bus, "sync-message::tag", G_CALLBACK(bus_message_cb), this);
		gst_object_unref(GST_OBJECT(bus));

		gst_element_set_state(pipeline_, GST_STATE_PAUSED);
		gst_element_get_state(pipeline_, NULL, NULL, GST_CLOCK_TIME_NONE);
	}

	InspectorPrivate::~InspectorPrivate() {
		gst_element_set_state(pipeline_, GST_STATE_NULL);
		gst_object_unref(GST_OBJECT(pipeline_));
	}

	/*
		Called whenever a message is posted on the bus
	*/
	void InspectorPrivate::bus_message(GstMessage * message) {
		switch (GST_MESSAGE_TYPE(message)) {
		case GST_MESSAGE_TAG:
			GstTagList * tags;
			gst_message_parse_tag(message, &tags);

			char * tag_string;

			if (gst_tag_list_get_string(tags, GST_TAG_TITLE, &tag_string)) {
				dprint("Found title");
				title_ = tag_string;
				g_free(tag_string);
			}

			if (gst_tag_list_get_string(tags, GST_TAG_ALBUM, &tag_string)) {
				dprint("Found album");
				album_ = tag_string;
				g_free(tag_string);
			}

			gst_tag_list_free(tags);
			break;
		default:
			;
		}
	}

	/*
		Called whenever a pad is added to a decode bin
	*/
	void InspectorPrivate::pad_added(GstElement * element, GstPad * pad) {
		GstCaps * caps = gst_pad_get_caps_reffed(pad);
		if (caps == NULL) {
			dprint("Error getting capabilities of pad");
		} else {
			GstStructure * str = gst_caps_get_structure(caps, 0);
			std::string type(gst_structure_get_name(str));

			if (type.find("audio") == 0) {
				dprint("Found audio stream");
				has_audio_ = true;
			} else if (type.find("video") == 0) {
				dprint("Found video stream");
				has_video_ = true;
			} else {
				dprint("Found other stream");
			}

			gst_caps_unref(caps);
		}

		GstElement * fake = gst_element_factory_make("fakesink", NULL);
		gst_bin_add(GST_BIN(pipeline_), fake);
		gst_element_link(element, fake);
		gst_element_sync_state_with_parent(fake);
	}

	/*
		Indicates whether this source has an audio stream
	*/
	bool InspectorPrivate::audio() const {
		return has_audio_;
	}

	/*
		Indicates whether this source has a video stream
	*/
	bool InspectorPrivate::video() const {
		return has_video_;
	}

	/*
		Returns any detected album name
	*/
	std::string InspectorPrivate::album() const {
		return album_;
	}

	/*
		Returns any detected track title
	*/
	std::string InspectorPrivate::title() const {
		return title_;
	}

	Inspector::Inspector(std::string uri)
		: p(new InspectorPrivate(uri.c_str())) {}

	Inspector::~Inspector() {
		delete p;
	}

	bool Inspector::audio() const {
		return p->audio();
	}

	bool Inspector::video() const {
		return p->video();
	}

	std::string Inspector::album() const {
		return std::move(p->album());
	}

	std::string Inspector::title() const {
		return std::move(p->title());
	}
}
