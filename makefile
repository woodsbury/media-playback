NAME = mp
DISPLAY_NAME = Media Player
VERSION = 0.1

DEFINES = DEBUG OPTIMISE
INCLUDES = /usr/include/atk-1.0 /usr/include/cairo /usr/include/clutter-1.0 /usr/include/gdk-pixbuf-2.0 \
	/usr/include/glib-2.0 /usr/include/gstreamer-0.10 /usr/include/gtk-3.0 /usr/include/json-glib-1.0 \
	/usr/include/pango-1.0 /usr/include/libxml2 /usr/lib/glib-2.0/include
DIRECTORIES = source source/core source/toolkit source/toolkit/interface
TEST_DIRECTORIES = tests
LIBRARIES = clutter-glx-1.0 clutter-gst-1.0 gtk-3 sqlite3

CPPFLAGS = $(foreach INCLUDE, $(INCLUDES), -isystem$(INCLUDE)) $(foreach DEFINE, $(DEFINES), -D$(DEFINE)) -Iinclude \
	-DNAME='"$(NAME)"' -DDISPLAY_NAME='"$(DISPLAY_NAME)"' -DVERSION='"$(VERSION)"'
CXXFLAGS = -Wall -Wextra -pedantic -pipe -std=c++0x -O3
LOADLIBES = $(foreach LIBRARY, $(LIBRARIES), -l$(LIBRARY))

SOURCES = $(wildcard $(patsubst %, %/*.cpp, $(DIRECTORIES)))
OBJECTS = $(patsubst %, %.o, $(basename $(SOURCES)))

TEST_SOURCES = $(wildcard $(patsubst %, %/*.cpp, $(TEST_DIRECTORIES)))
TEST_HEADERS = $(wildcard $(patsubst %, %/*.hpp, $(TEST_DIRECTORIES)))
TEST_OBJECTS = $(patsubst %, %.o, $(basename $(TEST_SOURCES)))
COMBINED_OBJECTS = $(filter-out source/main.o, $(OBJECTS)) $(TEST_OBJECTS)

$(NAME): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

$(NAME)_test: $(COMBINED_OBJECTS)
	$(CXX) $(LDFLAGS) $(COMBINED_OBJECTS) $(LOADLIBES) -o $@

$(TEST_OBJECTS): $(TEST_HEADERS) $(TEST_SOURCES)

.PHONY: clean test

clean:
	-rm -f $(OBJECTS) $(TEST_OBJECTS)

format:
	astyle --options=./astylerc -r './include/*.hpp' './source/*.cpp'

test: $(NAME)_test
	./$(NAME)_test
