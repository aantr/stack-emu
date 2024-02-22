CC = g++

CFLAGS += \
	-std=c++17 \
	-O2 \
	-Wall      \
	-Wextra    \
	-Werror \

LDFLAGS +=

CFLAGS += -I $(abspath include) \
		-I $(abspath stack)  \
		-I $(abspath src)  \
		-I $(abspath vector)  \
		-I $(abspath deque)  \
		-I $(abspath test) \
		-I $(abspath long_arithmetic/arithmetic) \
		-I $(abspath long_arithmetic/fft) \
		-I $(abspath common)

# what needs to be compiled
SOURCES = \
	test/test_system.cpp \
	src/compile.cpp \
	src/emulate.cpp \
	long_arithmetic/arithmetic/arithmetic.cpp \
	long_arithmetic/arithmetic/arithmetic_operators.cpp \
	long_arithmetic/fft/fft.cpp


# related to commands
SOURCE_EXECUTABLE = stack_emu.cpp
SOURCE_TEST = test.cpp

OBJECTS = $(SOURCES:%.cpp=build/%.o)

OBJECT_EXECUTABLE = build/src/stack_emu.o
OBJECT_TEST = build/test/test.o

EXECUTABLE_SOURCE = build/stack_emu.o
EXECUTABLE_TEST = build/test.o

build/%.o: %.cpp $(INCLUDES)
	@mkdir -p build/src
	@mkdir -p build/common
	@mkdir -p build/stack
	@mkdir -p build/vector
	@mkdir -p build/deque
	@mkdir -p build/test
	@mkdir -p build/long_arithmetic/arithmetic
	@mkdir -p build/long_arithmetic/fft
	$(CC) -c $< $(CFLAGS) -o $@

default: $(EXECUTABLE_SOURCE) $(EXECUTABLE_TEST)

$(EXECUTABLE_TEST): $(OBJECTS) $(OBJECT_TEST)
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJECT_TEST) -o $@

$(EXECUTABLE_SOURCE): $(OBJECTS) $(OBJECT_EXECUTABLE)
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJECT_EXECUTABLE) -o $@


run: $(EXECUTABLE_SOURCE)
	./$(EXECUTABLE_SOURCE)

test: $(EXECUTABLE_TEST)
	./$(EXECUTABLE_TEST)

clean:
	rm -rf build

.PHONY: run test clean default

# https://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

COMPILE.cpp = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o : %.cpp
%.o : %.cpp $(DEPDIR)/%.d | $(DEPDIR)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SOURCES:%.cpp=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))