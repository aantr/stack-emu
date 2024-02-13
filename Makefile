CC = g++

CFLAGS = \
	-std=c++17 \
	-O2 \
	-Wall      \
	-Wextra    \
	-Werror

LDFLAGS =

BRED    = \033[1;31m
BGREEN  = \033[1;32m
BYELLOW = \033[1;33m
GREEN   = \033[1;35m
BCYAN   = \033[1;36m
RESET   = \033[0m

INCLUDES = 
CFLAGS += -I $(abspath include) -I $(abspath stack) -I $(abspath test) 

SOURCES = \
	stack/stack.cpp \
	test/test_system.cpp


SOURCE_EXECUTABLE = stack_emu.cpp
SOURCE_TEST = test.cpp

OBJECTS = $(SOURCES:%.cpp=build/%.o)

OBJECT_EXECUTABLE = build/src/stack_emu.o
OBJECT_TEST = build/test/test.o

EXECUTABLE_SOURCE = build/stack_emu.o
EXECUTABLE_TEST = build/test.o

build/%.o: %.cpp $(INCLUDES)
	@printf "$(BYELLOW)Building object file $(BCYAN)$@$(RESET)\n"
	@mkdir -p build/src
	@mkdir -p build/stack
	@mkdir -p build/test
	$(CC) -c $< $(CFLAGS) -o $@

default: $(EXECUTABLE_SOURCE) $(EXECUTABLE_TEST)

$(EXECUTABLE_TEST): $(OBJECTS) $(OBJECT_TEST)
	@printf "$(BYELLOW)Linking executable $(BCYAN)$@$(RESET)\n"
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJECT_TEST) -o $@

$(EXECUTABLE_SOURCE): $(OBJECTS) $(OBJECT_EXECUTABLE)
	@printf "$(BYELLOW)Linking executable $(BCYAN)$@$(RESET)\n"
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJECT_EXECUTABLE) -o $@


run: $(EXECUTABLE_SOURCE)
	./$(EXECUTABLE_SOURCE)

test: $(EXECUTABLE_TEST)
	./$(EXECUTABLE_TEST)

clean:
	@printf "$(BYELLOW)Cleaning build directory $(RESET)\n"
	rm -rf build

.PHONY: run test clean default