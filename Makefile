
BUILD_DIR := ./build
SRC_DIRS := ./src
CC :=
CFLAGS :=
BINARY_BIN :=
FONT_DIR := fonts
FONT := dogicapixel.ttf
GFX_DIR := assets
GEAR_ICON := settings_icon.png
PLAY_ICON := play_icon.png
PAUSE_ICON := pause_icon.png
STOP_ICON := stop_icon.png
SEEK_ICON := seek_icon.png
TARGET :=

LINUX_TARGET := fftplayer
LINUX_CC := clang
LINUX_LDFLAGS := -Llinux_resources/SDL2-2.30.5/build/.libs -Llinux_resources/SDL2_ttf-2.22.0/.libs -Llinux_resources/SDL2_image-2.8.2/.libs -Llibsndfile-1.2.2/src/.libs -lSDL2 -lSDL2_ttf -lSDL2_image -lsndfile -lm -pg -fprofile-instr-generate
LIN_LIB_INC := -Ilinux_resources/SDL2-2.30.5/include -Ilinux_resources/libsndfile-1.2.2/include
LINUX_CFLAGS :=  -O2 -Wall -Wextra -pg -fprofile-instr-generate -fcoverage-mapping -g
LINUX_BIN := LINUX

WIN_TARGET := fftplayer.exe
WIN_CC := x86_64-w64-mingw32-gcc
WIN_LDFLAGS := -Lwin_resources/SDL2-2.30.5/x86_64-w64-mingw32/bin -Lwin_resources/SDL2_ttf-2.22.0/x86_64-w64-mingw32/bin -Lwin_resources/SDL2_image-2.8.2/x86_64-w64-mingw32/bin -Lwin_resources/libsndfile-1.2.2-win64/bin -Lwin_resources/x86_64-w64-mingw32/lib -lm -lmingw32 -mwindows -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lgdi32 -lsndfile
WIN_LIB_INC := -Iwin_resources/SDL2-2.30.5/include -Iwin_resources/libsndfile/include -Iwin_resources/x86_64-w64-mingw32/include
WIN_CFLAGS :=  -O2 -Wall -Wextra -g
WIN_BIN := WINDOWS

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS := $(INC_FLAGS) -MMD -MP

linux: CFLAGS = $(LINUX_CFLAGS)
linux: CC = $(LINUX_CC)
linux: CPPFLAGS += $(LIN_LIB_INC)
linux: $(BUILD_DIR)/$(LINUX_TARGET)
linux: BINARY_BIN := $(LINUX_BIN)
linux: TARGET := $(LINUX_TARGET)
linux: bin
linux: somove
linux: clean

windows: CFLAGS = $(WIN_CFLAGS)
windows: CC = $(WIN_CC)
windows: CPPFLAGS += $(WIN_LIB_INC)
windows: $(BUILD_DIR)/$(WIN_TARGET)
windows: BINARY_BIN := $(WIN_BIN)
windows: TARGET := $(WIN_TARGET)
windows: bin
windows: dllmove
windows: clean


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

somove:
	cp $(FONT_DIR)/$(FONT) $(BINARY_BIN)
	cp $(GFX_DIR)/$(GEAR_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(PLAY_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(PAUSE_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(STOP_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(SEEK_ICON) $(BINARY_BIN)
	mv $(BUILD_DIR)/$(TARGET) $(BINARY_BIN)

dllmove:
	cp $(FONT_DIR)/$(FONT) $(BINARY_BIN)
	cp $(GFX_DIR)/$(GEAR_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(PLAY_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(PAUSE_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(STOP_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(SEEK_ICON) $(BINARY_BIN)
	mv $(BUILD_DIR)/$(TARGET) $(BINARY_BIN)

clean:
	rm -r $(BUILD_DIR)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
