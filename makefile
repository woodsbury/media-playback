NAME = mp
VERSION = 0

DEFINES = DEBUG NAME='"$(NAME)"' VERSION='"$(VERSION)"'
INCLUDES = include
SYSTEM_INCLUDES =
DIRECTORIES = source source/core source/toolkit
TEST_DIRECTORIES = tests
LIBRARIES = sqlite3

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

$(NAME): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

$(NAME)_test: $(OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(TEST_OBJECTS) $(LOADLIBES) -o $@

$(TEST_OBJECTS): $(TEST_HEADERS) $(TEST_SOURCES)

.PHONY: clean test

clean:
	-rm $(OBJECTS) $(TEST_OBJECTS)

test: $(NAME)_test
	./$(NAME)_test
