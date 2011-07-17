NAME = mp
VERSION = 0

DEFINES = DEBUG NAME='"$(NAME)"' VERSION='"$(VERSION)"'
INCLUDES = include
SYSTEM_INCLUDES = /usr/include/atk-1.0 /usr/include/cairo /usr/include/clutter-1.0 /usr/include/json-glib-1.0 \
	/usr/include/pango-1.0
DIRECTORIES = source source/core source/toolkit
TEST_DIRECTORIES = tests
LIBRARIES = clutter-glx-1.0 clutter-gst-1.0 sqlite3

# GStreamer specifics
SYSTEM_INCLUDES += /usr/include/gstreamer-0.10 /usr/include/glib-2.0 /usr/lib/glib-2.0/include /usr/include/libxml2
DIRECTORIES += source/media/gstreamer
LIBRARIES += gstreamer-0.10

CPPFLAGS = $(foreach SYSTEM_INCLUDE, $(SYSTEM_INCLUDES), -isystem$(SYSTEM_INCLUDE)) \
	$(foreach INCLUDE, $(INCLUDES), -I$(INCLUDE)) $(foreach DEFINE, $(DEFINES), -D$(DEFINE))
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

test: $(NAME)_test
	./$(NAME)_test
