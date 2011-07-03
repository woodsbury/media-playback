NAME = mp
VERSION =

DEFINES =
INCLUDES =
SYSTEM_INCLUDES =
DIRECTORIES =
TEST_DIRECTORIES =

CPPFLAGS = $(foreach SYSTEM_INCLUDE, $(SYSTEM_INCLUDES), \
	-isystem$(SYSTEM_INCLUDE)) $(foreach INCLUDE, $(INCLUDES), -I$(INCLUDE)) \
	$(foreach DEFINE, $(DEFINES), -D$(DEFINE))
CXXFLAGS = -Wall -Wextra -pedantic -pipe -std=c++0x
LOADLIBES =

SOURCES = $(wildcard $(patsubst %,%/*.cpp,$(DIRECTORIES)))
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

TEST_SOURCES = $(wildcard $(patsubst %,%/*.cpp,$(TEST_DIRECTORIES)))
TEST_HEADERS = $(wildcard $(patsubst %,%/*.hpp,$(TEST_DIRECTORIES)))
TEST_OBJECTS = $(patsubst %.cpp,%.o,$(TEST_SOURCES))

$(NAME): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

$(NAME)_test: $(OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(TEST_OBJECTS) $(LOADLIBES) -o $@

.PHONY: clean test

clean:
	-rm $(OBJECTS)
	-rm $(TEST_OBJECTS)

test: $(NAME)_test
	./$(NAME)_test
