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

#include <core/filesystem.hpp>
#include <toolkit/inspector.hpp>

namespace test {
	namespace inspector {
		/*
			Test with invalid URI
		*/
		void invalidTest() {
			::toolkit::Inspector inspect("asdf://asdf");
			isFalse(inspect.audio());
			isFalse(inspect.video());
			isTrue(inspect.title().empty());
			isTrue(inspect.album().empty());
		}

		/*
			Test with audio file
		*/
		void audioTest() {
			::toolkit::Inspector inspect("file://" + ::core::Path::current() + "/tests/audio.ogg");
			isTrue(inspect.audio());
			isFalse(inspect.video());
			equal(inspect.title(), "Test Audio");
			equal(inspect.album(), "Test Album");
		}

		/*
			Test with video file
		*/
		void videoTest() {
			::toolkit::Inspector inspect("file://" + ::core::Path::current() + "/tests/video.ogg");
			isFalse(inspect.audio());
			isTrue(inspect.video());
			isTrue(inspect.title().empty());
			isTrue(inspect.album().empty());
		}

		/*
			Test with file with audio and video
		*/
		void multiTest() {
			::toolkit::Inspector inspect("file://" + ::core::Path::current() + "/tests/multi.ogg");
			isTrue(inspect.audio());
			isTrue(inspect.video());
		}

		void runTests() {
			invalidTest();
			audioTest();
			videoTest();
			multiTest();
		}
	}
}
