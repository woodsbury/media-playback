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
#include <media/elements.hpp>

extern "C" {
#include <gst/gst.h>
}

namespace {
	void message_cb(GstBus * bus, GstMessage * message, gpointer data);
	void file_pad_added_cb(GstElement * element, GstPad * pad, gpointer data);
	void play_pad_added_cb(GstElement * element, GstPad * pad, gpointer data);
}

namespace gstreamer {
/*
	Inheriting from this class will automatically initialise GStreamer
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

namespace media {
// Private class declarations
	class SourcePrivate
		: gstreamer::Initialiser {
	public:
		virtual ~SourcePrivate();

		virtual GstElement * createElement() const = 0;
	};

	class SinkPrivate
		: gstreamer::Initialiser {
	protected:
		GstElement * pipeline_;

		virtual GstElement * sink_element() = 0;

	public:
		SinkPrivate();
		virtual ~SinkPrivate();

		void connect(Source const & source);

		void eosReached();

		bool play();
		bool pause();
	};

// Source base class
	SourcePrivate::~SourcePrivate() {}

	Source::~Source() {
		delete p;
	}

// Sink base class
	SinkPrivate() {
		pipeline_ = gst_pipeline_new(NULL);

		GstBus * bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline_));
		gst_bus_enable_sync_message_emission(bus);
		g_signal_connect(bus, "sync-message", G_CALLBACK(message_cb), this);
		gst_object_unref(bus);
	}

	virtual ~SinkPrivate() {
		gst_element_set_state(pipeline_, GST_STATE_NULL);
		gst_object_unref(pipeline_);
	}

/*
	Configure this sink to play from the given source
*/
	void connect(Source const & source) {
		GstElement * source_element = source.p->createElement();
		gst_bin_add(GST_BIN(pipeline_), source_element);
		gst_element_sync_state_with_parent(source_element);

		gst_element_link(source_element, sink_element());
	}

/*
	Called when the sink has reached the end of the stream
*/
	void eosReached() {
		gst_element_set_state(pipeline_, GST_STATE_PAUSED);
		gst_element_seek(pipeline_, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET, 0,
			GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
		gst_element_get_state(pipeline_, NULL, NULL, GST_CLOCK_TIME_NONE);
	}

/*
	Start playing back pipeline
*/
	bool play() {
		gst_element_set_state(pipeline_, GST_STATE_PLAYING);
		return gst_element_get_state(pipeline_, NULL, NULL, GST_CLOCK_TIME_NONE) != GST_STATE_CHANGE_FAILURE;
	}

/*
	Stop playing back pipeline
*/
	bool pause() {
		gst_element_set_state(pipeline_, GST_STATE_PAUSED);
		return gst_element_get_state(pipeline_, NULL, NULL, GST_CLOCK_TIME_NONE) != GST_STATE_CHANGE_FAILURE;
	}

	Sink::~Sink() {
		delete p;
	}

	bool Sink::play() {
		return p->play();
	}

	bool Sink::pause() {
		return p->pause();
	}

// Local file source
	class FileSourcePrivate
		: public SourcePrivate {
		std::string const location_;

	public:
		FileSourcePrivate(std::string location)
			: location_(location) {}

		GstElement * createElement() const {
			GstElement * element = gst_element_factory_make("filesrc", NULL);
			g_object_set(element, "location", location_.c_str(), NULL);

			return element;
		}
	};

	FileSource::FileSource(std::string location) {
		p = new FileSourcePrivate(location);
	}

// HTTP file source
	class HttpSourcePrivate
		: public SourcePrivate {
		std::string location_;

	public:
		HttpSourcePrivate(std::string location) {
			if (location.find("http://") == 0) {
				location_ = location;
			} else {
				location_ = "http://" + location;
			}
		}

		GstElement * createElement() const {
			GstElement * element = gst_element_factory_make("souphttpsrc", NULL);
			g_object_set(element, "location", location_.c_str(), NULL);

			return element;
		}
	};

	HttpSource::HttpSource(std::string location) {
		p = new HttpSourcePrivate(location);
	}

// Local file sink
	class FileSinkPrivate
		: public SinkPrivate {
		std::string const location_;

		std::string const container_name_;
		std::string const audio_name_;
		std::string const video_name_;

		GstElement * container_element_;

		GstElement * sink_element() {
			GstElement * decode_element = gst_element_factory_make("decodebin2", NULL);
			gst_bin_add(GST_BIN(pipeline_), decode_element);
			g_signal_connect(decode_element, "pad-added", G_CALLBACK(file_pad_added_cb), this);

			container_element_ = gst_element_factory_make(container_name_.c_str(), NULL);
			gst_bin_add(GST_BIN(pipeline_), container_element_);

			GstElement * sink_element = gst_element_factory_make("filesink", NULL);
			g_object_set(sink_element, "location", location_.c_str(), NULL);
			gst_bin_add(GST_BIN(pipeline_), sink_element);

			gst_element_link(container_element_, sink_element);

			return decode_element;
		}

	public:
		FileSinkPrivate(std::string location, std::string containerName, std::string audioName, std::string videoName)
			: location_(location), container_name_(containerName), audio_name_(audioName), video_name_(videoName) {}

		void audioPadAdded(GstElement * element) {
			GstElement * encode_element = gst_element_factory_make(audio_name_.c_str(), NULL);
			GstElement * queue_element = gst_element_factory_make("queue", NULL);
			gst_bin_add(GST_BIN(pipeline_), queue_element);
			gst_bin_add(GST_BIN(pipeline_), encode_element);
			gst_element_link_many(element, queue_element, encode_element, container_element_, NULL);
			gst_element_sync_state_with_parent(queue_element);
			gst_element_sync_state_with_parent(encode_element);
		}

		void videoPadAdded(GstElement * element) {
			GstElement * encode_element = gst_element_factory_make(video_name_.c_str(), NULL);
			GstElement * queue_element = gst_element_factory_make("queue", NULL);
			gst_bin_add(GST_BIN(pipeline_), queue_element);
			gst_bin_add(GST_BIN(pipeline_), encode_element);
			gst_element_link_many(element, queue_element, encode_element, container_element_, NULL);
			gst_element_sync_state_with_parent(queue_element);
			gst_element_sync_state_with_parent(encode_element);
		}
	};

	FileSink::FileSink(Source const & source, std::string location, Container container, Audio audio, Video video) {
		std::string container_element;
		switch (container) {
			case Container::Matroska:
				container_element = "matroskamux";
				break;
			case Container::Ogg:
				container_element = "oggmux";
				break;
		}

		std::string audio_element;
		switch (audio) {
			case Audio::Vorbis:
				audio_element = "vorbisenc";
				break;
		}

		std::string video_element;
		switch (video) {
			case Video::Theora:
				video_element = "theoraenc";
				break;
		}

		p = new FileSinkPrivate(location, container_element, audio_element, video_element);
		p->connect(source);
	}

// Local playback sink
	class PlaySinkPrivate
		: public SinkPrivate {
		GstElement * sink_element() {
			GstElement * decode_element = gst_element_factory_make("decodebin2", NULL);
			gst_bin_add(GST_BIN(pipeline_), decode_element);
			g_signal_connect(decode_element, "pad-added", G_CALLBACK(play_pad_added_cb), this);

			return decode_element;
		}

	public:
		void audioPadAdded(GstElement * element) {
			GstElement * sink_element = gst_element_factory_make("autoaudiosink", NULL);
			GstElement * queue_element = gst_element_factory_make("queue", NULL);
			gst_bin_add(GST_BIN(pipeline_), queue_element);
			gst_bin_add(GST_BIN(pipeline_), sink_element);
			gst_element_link_many(element, queue_element, sink_element, NULL);
			gst_element_sync_state_with_parent(sink_element);
			gst_element_sync_state_with_parent(queue_element);
		}

		void videoPadAdded(GstElement * element) {
			GstElement * sink_element = gst_element_factory_make("xvimagesink", NULL);
			GstElement * queue_element = gst_element_factory_make("queue", NULL);
			gst_bin_add(GST_BIN(pipeline_), sink_element);
			gst_bin_add(GST_BIN(pipeline_), queue_element);
			gst_element_link_many(element, queue_element, sink_element, NULL);
			gst_element_sync_state_with_parent(sink_element);
			gst_element_sync_state_with_parent(queue_element);
		}
	};

	PlaySink::PlaySink(Source const & source) {
		p = new PlaySinkPrivate;
		p->connect(source);
	}
}

namespace {
// Callbacks
/*
	A message has been received on the bus
*/
	void message_cb(GstBus *, GstMessage * message, gpointer data) {
		switch (GST_MESSAGE_TYPE(message)) {
			case GST_MESSAGE_EOS:
				dprint("End of stream reached");
				reinterpret_cast< media::SinkPrivate * >(data)->eosReached();
				break;
			default:
				;
		}
	}

/*
	A new pad has been added to the FileSink's decoder
*/
	void file_pad_added_cb(GstElement * element, GstPad * pad, gpointer data) {
		GstCaps * caps = gst_pad_get_caps(pad);

		GstStructure * caps_str = gst_caps_get_structure(caps, 0);
		std::string pad_type(gst_structure_get_name(caps_str));

		if (pad_type.find("audio") == 0) {
			reinterpret_cast< media::FileSinkPrivate * >(data)->audioPadAdded(element);
		} else if (pad_type.find("video") == 0) {
			reinterpret_cast< media::FileSinkPrivate * >(data)->videoPadAdded(element);
		} else {
			dprint("Unknown pad type");
		}

		gst_caps_unref(caps);
	}

/*
	A new pad has been added to the PlaySink's decoder
*/
	void play_pad_added_cb(GstElement * element, GstPad * pad, gpointer data) {
		GstCaps * caps = gst_pad_get_caps(pad);

		GstStructure * caps_str = gst_caps_get_structure(caps, 0);
		std::string pad_type(gst_structure_get_name(caps_str));

		if (pad_type.find("audio") == 0) {
			reinterpret_cast< media::PlaySinkPrivate * >(data)->audioPadAdded(element);
		} else if (pad_type.find("video") == 0) {
			reinterpret_cast< media::PlaySinkPrivate * >(data)->videoPadAdded(element);
		} else {
			dprint("Unknown pad type");
		}

		gst_caps_unref(caps);
	}
}
