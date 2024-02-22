BUILD_DIR = build

CC = g++

CFLAGS += \
	-std=c++17 \
	-O2 \
	-Wall      \
	-Wextra    \
	-Werror \

LDFLAGS +=

CFLAGS += -I $(abspath stack)  \
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
	long_arithmetic/fft/fft.cpp \


OBJECTS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)

OBJECT_STACK_EMU = $(BUILD_DIR)/src/stack_emu.o
OBJECT_TEST = $(BUILD_DIR)/test/test.o

EXECUTABLE_STACK_EMU = $(BUILD_DIR)/stack_emu_
EXECUTABLE_TEST = $(BUILD_DIR)/test_

# https://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
DEPDIR := $(BUILD_DIR)/.deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

COMPILE.cpp = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

$(BUILD_DIR)/%.o: %.cpp $(DEPDIR)/%.d | $(DEPDIR)
	@mkdir -p `echo $@ | cut -b -\`expr \\\`echo $@ | awk '{print length}'\\\` - \\\`echo $@ | rev | cut -d / -f 1 | awk '{print length}'\\\` - 1\``
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
$(BUILD_DIR)/%.o: %.cpp $(DEPDIR)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SOURCES:%.cpp=$(DEPDIR)/%.d)
$(DEPFILES):
# # #

$(EXECUTABLE_TEST): $(OBJECTS) $(OBJECT_TEST)
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJECT_TEST) -o $@

$(EXECUTABLE_STACK_EMU): $(OBJECTS) $(OBJECT_STACK_EMU)
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJECT_STACK_EMU) -o $@

build: $(EXECUTABLE_TEST) $(EXECUTABLE_STACK_EMU)

run: $(EXECUTABLE_STACK_EMU)
	./$(EXECUTABLE_STACK_EMU)

test: $(EXECUTABLE_TEST)
	./$(EXECUTABLE_TEST)

clean:
	rm -rf build

.PHONY: run test clean build

include $(wildcard $(DEPFILES))