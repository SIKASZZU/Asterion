# --- Project Settings ---
TARGET_EXEC := main
BIN_DIR     := bin
BLD_DIR     := bld
SRC_DIR     := src
INC_DIR     := include
ASSET_DIR   := assets

TARGET      := $(BIN_DIR)/$(TARGET_EXEC)

# --- Compiler & Flags ---
CXX         := g++
CXXFLAGS    := -std=c++20 -Wall -Wextra -g
CPPFLAGS    := -I$(INC_DIR) -MMD -MP

# Fetch SDL3 settings dynamically
SDL_CFLAGS  := $(shell pkg-config --cflags sdl3 sdl3-image)
SDL_LIBS    := $(shell pkg-config --libs sdl3 sdl3-image)

# Combine flags
CPPFLAGS    += $(SDL_CFLAGS)
LDLIBS      := $(SDL_LIBS)

# --- Discovery ---
# Finds all .cpp files in src/ and its subdirectories
SOURCES     := $(shell find $(SRC_DIR) -type f -name '*.cpp')
# Maps src/path/file.cpp -> bld/path/file.o
OBJECTS     := $(SOURCES:$(SRC_DIR)/%.cpp=$(BLD_DIR)/%.o)
DEPENDS     := $(OBJECTS:.o=.d)

# --- Rules ---
.PHONY: all build run clean copy_assets

all: build copy_assets

build: $(TARGET)

run: build copy_assets
	./$(TARGET)

# Linking the final executable
$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) $(OBJECTS) -o $@ $(LDLIBS)

# Compiling source files
$(BLD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Simple Asset Sync: Copy the whole assets folder to bin/
copy_assets:
	@echo "Syncing assets..."
	@mkdir -p $(BIN_DIR)/assets
	@cp -ru $(ASSET_DIR)/* $(BIN_DIR)/assets/ 2>/dev/null || true

clean:
	@echo "Cleaning project..."
	$(RM) -r $(BLD_DIR) $(BIN_DIR)

-include $(DEPENDS)