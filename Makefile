# Этот Makefile заставит твой проект --найти все зависимости и заголовки-- за 3 секунды! Тебе лишь нужно ...
# Прописать пути ко всем .cpp файлам
# https://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
# https://bnikolic.co.uk/blog/sh/make/unix/2021/07/08/makefile
# https://stackoverflow.com/questions/2214575/passing-arguments-to-make-run
# auto find headers & dependencies & FLAGS changes & Makefile changes
# build c++ project for unix (Darwin or Linux)
# need change only compiler settings & sources

BUILD_DIR = build
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
SHA256COMMAND = sha256sum
endif
ifeq ($(UNAME), Darwin)
SHA256COMMAND = shasum -a 256
endif
DEPDIR := $(BUILD_DIR)/.deps
MAKEFILE_JUSTNAME := $(firstword $(MAKEFILE_LIST))
MAKEFILE_COMPLETE := $(CURDIR)/$(MAKEFILE_JUSTNAME)

# # if necessary
# # Use the rest as arguments
# RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
# # ...and turn them into do-nothing targets
# $(eval $(RUN_ARGS):;@:)
ifeq ($(UNAME), Darwin)
CXX = g++-13
endif
ifeq ($(UNAME), Linux)
CXX = g++
endif

CXXFLAGS += \
	-std=c++23 \
	-Ofast \
	-Wall      \
	-Wextra    \
	-Werror \

LDFLAGS +=

INCLUDES = 

# Find dependencies, headers, compile
CXXFLAGS += $(foreach f,$(INCLUDES),-I $(abspath $f))
CXXFLAGS += $(foreach fn,$(shell find . -type f -name "*.hpp"),-I $(shell realpath `dirname $(fn)`))
CXXFLAGS += $(foreach fn,$(shell find . -type f -name "*.h"),-I $(shell realpath `dirname $(fn)`))

ALLFLAGS := $(CXX) + $(foreach v,$(filter %FLAGS,$(.VARIABLES)),$($(v)))
FLAGHASH := $(strip $(shell echo $(ALLFLAGS) | $(SHA256COMMAND) | cut -d " " -f1 ))
FLAGHASH := $(FLAGHASH)-$(strip $(shell cat $(MAKEFILE_COMPLETE) | tr -d '\n' | tr -d '\t' | tr -d ' ' | $(SHA256COMMAND) | cut -d " " -f1 ))

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

$(BUILD_DIR)/%-$(FLAGHASH).o: %.cpp $(DEPDIR)/%.d $(DEPDIR)/%.d | $(DEPDIR)
	@mkdir -p  $(DEPDIR)/`dirname $<`
	@mkdir -p  `dirname $@`
	@echo "Building $<"
	@$(COMPILE.cpp) $(OUTPUT_OPTION) $<

# Commands

SOURCES_TEST = \
	test/test_system.cpp \
	test/test.cpp  \
	long_arithmetic/arithmetic/arithmetic.cpp \
	long_arithmetic/arithmetic/arithmetic_operators.cpp \
	long_arithmetic/fft/fft.cpp \

SOURCES_STACK_EMU = \
	src/compile.cpp \
	src/emulate.cpp \
	src/stack_emu.cpp \
	src/parser.cpp \
	long_arithmetic/arithmetic/arithmetic.cpp \
	long_arithmetic/arithmetic/arithmetic_operators.cpp \
	long_arithmetic/fft/fft.cpp \

ALL_SOURCES = $(SOURCES_STACK_EMU) $(SOURCES_TEST)

APP_STACK_EMU = $(BUILD_DIR)/stack_emu_
APP_TEST = $(BUILD_DIR)/test_

OBJECTS_TEST = $(foreach f,$(SOURCES_TEST),$(BUILD_DIR)/$(basename $f)-$(FLAGHASH).o)
OBJECTS_STACK_EMU = $(foreach f,$(SOURCES_STACK_EMU),$(BUILD_DIR)/$(basename $f)-$(FLAGHASH).o)

$(APP_TEST): $(OBJECTS_TEST)
	@echo "Linking $@"
	@$(CXX) $(LDFLAGS) $(OBJECTS_TEST) -o $@

$(APP_STACK_EMU): $(OBJECTS_STACK_EMU)
	@echo "Linking $@"
	@$(CXX) $(LDFLAGS) $(OBJECTS_STACK_EMU) -o $@

build: $(APP_TEST) $(APP_STACK_EMU)

run: $(APP_STACK_EMU)
# 	./$(APP_STACK_EMU) $(RUN_ARGS)

test: $(APP_TEST)
# 	./$(APP_TEST) $(RUN_ARGS)

clean:
	rm -rf build

.PHONY: run test clean build

$(DEPDIR): ; @mkdir -p $@
DEPFILES := $(ALL_SOURCES:%.cpp=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))
