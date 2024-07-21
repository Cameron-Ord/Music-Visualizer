
BUILD_DIR := ./build
SRC_DIRS := ./src
CC :=
CFLAGS :=
BINARY_BIN :=
FONT := dogicapixel.ttf
GFX_DIR := assets
GEAR_ICON := settings_icon.png
PLAY_ICON := play_icon.png
PAUSE_ICON := pause_icon.png
STOP_ICON := stop_icon.png
SEEK_ICON := seek_icon.png
TARGET :=

WIN_TARGET := fftplayer.exe
WIN_CC := x86_64-w64-mingw32-gcc
WIN_LDFLAGS := -Lwin_resources/SDL2-2.30.5/x86_64-w64-mingw32/lib -Lwin_resources/SDL2_ttf-2.22.0/x86_64-w64-mingw32/lib -Lwin_resources/SDL2_image-2.8.2/x86_64-w64-mingw32/lib -Lwin_resources/libsndfile-1.2.2-win64/lib -lm -lmingw32 -mwindows -lSDL2main -lSDL2  -lSDL2_ttf -lSDL2_image -lsndfile -lgdi32 
WIN_LIB_INC := -Iwin_resources/SDL_HEADERS/include -Iwin_resources/LIBSNDFILE_HEADER/include
WIN_CFLAGS :=  -O2 -Wall -Wextra -g
WIN_BIN := WINDOWS

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS := $(INC_FLAGS) -MMD -MP

CFLAGS = $(WIN_CFLAGS)
CC = $(WIN_CC)
CPPFLAGS += $(WIN_LIB_INC)
BINARY_BIN := $(WIN_BIN)
TARGET := $(WIN_TARGET)

all:	$(BUILD_DIR)/$(WIN_TARGET)

# The final build step.
$(BUILD_DIR)/$(LINUX_TARGET): $(OBJS)
	$(LINUX_CC) $(OBJS) -o $@ $(LINUX_LDFLAGS) 

$(BUILD_DIR)/$(WIN_TARGET): $(OBJS)
	$(WIN_CC) $(OBJS) -o $@ $(WIN_LDFLAGS) 

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

bin:
	mkdir -p $(BINARY_BIN)

bin_resources:
	cp $(GFX_DIR)/$(FONT) $(BINARY_BIN)
	cp $(GFX_DIR)/$(GEAR_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(PLAY_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(PAUSE_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(STOP_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(SEEK_ICON) $(BINARY_BIN)
	mv $(BUILD_DIR)/$(TARGET) $(BINARY_BIN)
	cp -r win_resources/SDL2-2.30.5/x86_64-w64-mingw32/bin/*.dll $(BINARY_BIN)
	cp -r win_resources/SDL2_ttf-2.22.0/x86_64-w64-mingw32/bin/*.dll $(BINARY_BIN)
	cp -r win_resources/libsndfile-1.2.2-win64/bin/*.dll $(BINARY_BIN)
	cp -r win_resources/SDL2_image-2.8.2/x86_64-w64-mingw32/bin/*.dll $(BINARY_BIN)
	cp -r LICENSES $(BINARY_BIN)

install: bin bin_resources clean

clean:
	rm -r $(BUILD_DIR)

.PHONY: install bin bin_resources clean

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
