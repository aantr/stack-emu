# https://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
# https://bnikolic.co.uk/blog/sh/make/unix/2021/07/08/makefile
# auto find headers & dependencies & FLAGS changes
# build c++ project

BUILD_DIR = build
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
SHA256COMMAND = sha256sum
endif
ifeq ($(UNAME), Darwin)
SHA256COMMAND = shasum -a 256
endif
DEPDIR := $(BUILD_DIR)/.deps

CXX = g++

CXXFLAGS += \
	-std=c++17 \
	-O3 \
	-Wall      \
	-Wextra    \
	-Werror \

LDFLAGS +=

INCLUDES = 

SOURCES = \
	test/test_system.cpp \
	src/compile.cpp \
	src/emulate.cpp \
	long_arithmetic/arithmetic/arithmetic.cpp \
	long_arithmetic/arithmetic/arithmetic_operators.cpp \
	long_arithmetic/fft/fft.cpp \

SOURCE_TEST = test/test.cpp
SOURCE_STACK_EMU = src/stack_emu.cpp
ALL_SOURCES = $(SOURCE_STACK_EMU) $(SOURCE_TEST) $(SOURCES)

APP_STACK_EMU = $(BUILD_DIR)/stack_emu_
APP_TEST = $(BUILD_DIR)/test_

CXXFLAGS += $(foreach f,$(INCLUDES),-I $(abspath $f))
CXXFLAGS += $(foreach fn,$(shell find . -type f -name "*.hpp"),-I $(shell realpath `dirname $(fn)`))
CXXFLAGS += $(foreach fn,$(shell find . -type f -name "*.h"),-I $(shell realpath `dirname $(fn)`))

ALLFLAGS := $(CXX) + $(foreach v,$(filter %FLAGS,$(.VARIABLES)),$($(v)))
FLAGHASH := $(strip $(shell echo $(ALLFLAGS) | $(SHA256COMMAND) | cut -d " " -f1 ))

OBJECTS := $(foreach f,$(SOURCES),$(BUILD_DIR)/$(basename $f)-$(FLAGHASH).o)
OBJECT_STACK_EMU = $(foreach f,$(SOURCE_STACK_EMU),$(BUILD_DIR)/$(basename $f)-$(FLAGHASH).o)
OBJECT_TEST = $(foreach f,$(SOURCE_TEST),$(BUILD_DIR)/$(basename $f)-$(FLAGHASH).o)

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

$(BUILD_DIR)/%-$(FLAGHASH).o: %.cpp $(DEPDIR)/%.d $(DEPDIR)/%.d # | $(DEPDIR)
	@mkdir -p $(DEPDIR)/`echo $@ | cut -b \`expr 1 + \\\`echo $@ | cut -d / -f 1 | awk '{print length}'\\\`\`-\`expr \\\`echo $@ | awk '{print length}'\\\` - \\\`echo $@ | rev | cut -d / -f 1 | awk '{print length}'\\\` - 1\``
	@mkdir -p `echo $@ | cut -b -\`expr \\\`echo $@ | awk '{print length}'\\\` - \\\`echo $@ | rev | cut -d / -f 1 | awk '{print length}'\\\` - 1\``
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(ALL_SOURCES:%.cpp=$(DEPDIR)/%.d)
$(DEPFILES):

# app commands
$(APP_TEST): $(OBJECTS) $(OBJECT_TEST)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(OBJECT_TEST) -o $@

$(APP_STACK_EMU): $(OBJECTS) $(OBJECT_STACK_EMU)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(OBJECT_STACK_EMU) -o $@

build: $(APP_TEST) $(APP_STACK_EMU)

run: $(APP_STACK_EMU)
	./$(APP_STACK_EMU)

test: $(APP_TEST)
	./$(APP_TEST)

clean:
	rm -rf build

.PHONY: run test clean build

include $(wildcard $(DEPFILES))