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

#include <cstdio>
#include <media.hpp>

extern "C" {
#include <unistd.h>
}

namespace test { namespace media {
	void playFile() {
		::media::FileSource file("./tests/test.ogg");
		::media::PlaySink play(file);
		isTrue(play.play());
		sleep(1);
		isTrue(play.pause());
		sleep(1);
		isTrue(play.play());
		isTrue(play.play());
		sleep(1);
		isTrue(play.pause());
	}

	void fileNonExist() {
		::media::FileSource file("./aaaaaaaaaaaaaaaa");
		::media::PlaySink play(file);
		isFalse(play.play());
		isFalse(play.pause());
	}

	void playHttp() {
		::media::HttpSource http("www.archive.org/download/The_Phoenix_Trap_self_titled/pt2002-01-18t12.ogg");
		::media::PlaySink play(http);
		isTrue(play.play());
		sleep(2);
		isTrue(play.pause());
	}

	void httpNonExist() {
		::media::HttpSource http("aaaaaaaaaaaaaaaa");
		::media::PlaySink play(http);
		isFalse(play.play());
		isFalse(play.pause());
	}

	void convertHttp() {
		{
			::media::HttpSource http("www.archive.org/download/The_Phoenix_Trap_self_titled/pt2002-01-18t12.ogg");
			::media::FileSink file(http, "./tests/convert_result.ogg", ::media::FileSink::OggContainer,
				::media::FileSink::VorbisAudio, ::media::FileSink::TheoraVideo);
			isTrue(file.play());
			isTrue(file.pause());
			isTrue(file.play());
		}
		equal(std::remove("./tests/convert_result.ogg"), 0);
	}

	void runTests() {
		playFile();
		fileNonExist();
		playHttp();
		convertHttp();
	}
}}
