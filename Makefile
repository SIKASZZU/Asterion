# Uncomment the next two lines if you run make from cmd.exe and want bash utilities (find, etc)
# SHELL := /usr/bin/env bash
# .SHELLFLAGS := -c

TARGET_EXEC ?= main
SRC_DIR ?= src
BUILD_DIR ?= bld
BIN_DIR ?= bin

CXX ?= g++
CXXFLAGS ?= -std=c++17

TARGET := $(BIN_DIR)/$(TARGET_EXEC)

# recursively find all .cpp under src (works in MSYS / bash)
SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp' 2>/dev/null | sort)

# map src/path/file.cpp -> bld/path/file.o  (preserves directory layout)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# dependency files
DEPENDENCIES := $(OBJECTS:.o=.d)

# auto-discover any "headers" directories under src and create -I flags
HEADER_DIRS := $(shell find $(SRC_DIR) -type d -name headers 2>/dev/null || true)
INCLUDE_FLAGS := $(patsubst %,-I%,$(HEADER_DIRS))

# dependency generation + include directories
CPPFLAGS := -MMD -MP -g $(INCLUDE_FLAGS)

# Put SDL libs or others here. MinGW often needs -lmingw32 before SDL to resolve entry points.
LDFLAGS := -lSDL3 -lSDL3_image

all: build run

build: $(TARGET) copy_resources

run: build
	cd $(dir $(TARGET)) && ./$(TARGET_EXEC)

# final link step: link all objects; add -lmingw32 before SDL libs on MinGW if necessary
$(TARGET): $(OBJECTS)
	mkdir -p $(dir $@)
	$(CXX) $(OBJECTS) -o $@ -lmingw32 $(LDFLAGS) -Wl,-subsystem,console

# compile rule: preserve src/ subdirs into bld/ subdirs
# example: src/logic/foo.cpp -> bld/logic/foo.o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# resource map (keeps your original resource copying logic)
RESOURCE_MAP := $(SRC_DIR)/render/headers/assets.hpp
RESOURCE_FILES := $(shell \
	[ -f $(RESOURCE_MAP) ] && \
	grep -E '^\s*[^/]*".*"' $(RESOURCE_MAP) 2>/dev/null | \
	sed -nE 's/.*"([^"]+)".*/\1/p' || true \
)
RESOURCE_FILES_IN_BLD := $(RESOURCE_FILES:%=$(BIN_DIR)/%)

copy_resources: $(RESOURCE_FILES_IN_BLD)
	@echo All files copied that were found in \"$(RESOURCE_MAP)\"

$(RESOURCE_FILES_IN_BLD): $(RESOURCE_FILES)
	@SRC=$(patsubst $(BIN_DIR)/%,%,$@);\
	echo "Copying $$SRC to $@";\
	mkdir -p $(dir $@);\
	cp $$SRC $@

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(BIN_DIR)

.PHONY: clean all run copy_resources print-vars

# include dependency files if generated
-include $(DEPENDENCIES)

# debug helper to see what Make discovered
print-vars:
	@echo "SOURCES:"; printf "  %s\n" $(SOURCES)
	@echo "OBJECTS:"; printf "  %s\n" $(OBJECTS)
	@echo "HEADER_DIRS:"; printf "  %s\n" $(HEADER_DIRS)
	@echo "INCLUDE_FLAGS: $(INCLUDE_FLAGS)"