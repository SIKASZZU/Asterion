# Useful links:
# - https://makefiletutorial.com/
# - https://www.gnu.org/software/make/manual/html_node/index.html#SEC_Contents
# MAKE SURE TO USE TABS	...

TARGET_EXEC ?= main
SRC_DIR ?= src
BUILD_DIR ?= bld
BIN_DIR ?= bin

TARGET := $(BIN_DIR)/$(TARGET_EXEC)
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:%=$(BUILD_DIR)/%.o)
DEPENDENDIES = $(OBJECTS:.o=.d)

RESOURCE_MAP := $(SRC_DIR)/assets.hpp
RESOURCE_FILES := $(shell \
	grep -E '^\s*[^/]*".*"' $(RESOURCE_MAP) | \
	sed -nE 's/.*"([^"]+)".*/\1/p' \
)
RESOURCE_FILES_IN_BLD := $(RESOURCE_FILES:%=$(BIN_DIR)/%)

# CXX := g++
# CXXFLAGS := -Isrc/lib
CPPFLAGS := -MMD -MP -g
LDFLAGS := -lSDL3 -lSDL3_image

all: build run

build: $(TARGET) copy_resources

run: build
# this is so that it uses the assets in the folder where the executable is
	cd $(dir $(TARGET)) && ./$(TARGET_EXEC)

# final build step
$(TARGET): $(OBJECTS)
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

# compile C++ source
# $(OBJECTS): $(SOURCES) will not work
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

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

.PHONY: clean all var_data run copy_resources

-include $(DEPENDENDIES)
