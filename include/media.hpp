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

	class Source {
		friend class SinkPrivate;

	protected:
		SourcePrivate * p;
	};

	class Sink {
	protected:
		SinkPrivate * p;

	public:
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
	Plays a source locally
*/
	class PlaySink
		: public Sink {
	public:
		PlaySink(Source & source);
	};
}

#endif
