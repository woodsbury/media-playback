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

#ifndef _MEDIA_HPP
#define _MEDIA_HPP

#include <string>

namespace media {
	class SourcePrivate;
	class SinkPrivate;

/*
	Base class for sources
*/
	class Source {
		friend class SinkPrivate;

	protected:
		SourcePrivate * p;

	public:
		~Source();
	};

/*
	Base class for sinks
*/
	class Sink {
	protected:
		SinkPrivate * p;

	public:
		~Sink();

		bool play();
		bool pause();
	};

/*
	Represents a local file
*/
	class FileSource
		: public Source {
	public:
		FileSource(std::string location);
	};

/*
	Represents a file from an HTTP server
*/
	class HttpSource
		: public Source {
	public:
		HttpSource(std::string location);
	};

/*
	Encodes a source and stores it in a file
*/
	class FileSink
		: public Sink {
	public:
		enum Container {
			Ogg
		};

		enum Audio {
			Vorbis
		};

		enum Video {
			Theora
		};

		FileSink(Source const & source, std::string location, Container container, Audio audio, Video video);
	};

/*
	Plays a source locally
*/
	class PlaySink
		: public Sink {
	public:
		PlaySink(Source const & source);
	};
}

#endif
