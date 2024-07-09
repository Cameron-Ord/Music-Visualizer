
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

LINUX_TARGET := fftplayer
LINUX_CC := clang
LINUX_LDFLAGS := -Llinux_resources/SDL2 -Llinux_resources/libsndfile -lSDL2 -lSDL2_ttf -lSDL2_image -lsndfile -lm 
LIN_LIB_INC := -Ilinux_resources/SDL2/include -Ilinux_resources/libsndfile/include
LINUX_CFLAGS :=  -O2 -Wall -Wextra -g
LINUX_BIN := LINUX

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
	cp $(GFX_DIR)/$(FONT) $(BINARY_BIN)
	cp $(GFX_DIR)/$(GEAR_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(PLAY_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(PAUSE_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(STOP_ICON) $(BINARY_BIN)
	cp $(GFX_DIR)/$(SEEK_ICON) $(BINARY_BIN)
	mv $(BUILD_DIR)/$(TARGET) $(BINARY_BIN)
	mkdir -p $(BINARY_BIN)/LIBS
	cp -r LICENSES $(BINARY_BIN)
	cp run.sh $(BINARY_BIN)
	chmod +x $(BINARY_BIN)/run.sh
	cp linux_resources/SDL2_ttf-2.22.0/.libs/libSDL2_ttf.so $(BINARY_BIN)/LIBS
	cp linux_resources/SDL2_ttf-2.22.0/.libs/libSDL2_ttf-2.0.so.0.2200.0 $(BINARY_BIN)/LIBS
	cp linux_resources/SDL2_image-2.8.2/.libs/libSDL2_image.so  $(BINARY_BIN)/LIBS
	cp linux_resources/SDL2_image-2.8.2/.libs/libSDL2_image-2.0.so.0.800.2  $(BINARY_BIN)/LIBS
	cp linux_resources/SDL2-2.30.5/build/.libs/libSDL2.so  $(BINARY_BIN)/LIBS
	cp linux_resources/SDL2-2.30.5/build/.libs/libSDL2-2.0.so.0.3000.5  $(BINARY_BIN)/LIBS
	cp linux_resources/libsndfile-1.2.2/src/.libs/libsndfile.so $(BINARY_BIN)/LIBS
	cp linux_resources/libsndfile-1.2.2/src/.libs/libsndfile.so.1.0.37 $(BINARY_BIN)/LIBS


dllmove:
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



clean:
	rm -r $(BUILD_DIR)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
