
BUILD_DIR := ./build
SRC_DIRS := ./src
CC :=
CFLAGS :=
BINARY_BIN :=
FONT_DIR := fonts
FONT := Roboto-Regular.ttf
TARGET :=


LINUX_TARGET := fftplayer
LINUX_CC := clang
LINUX_LDFLAGS := -lSDL2 -lSDL2_ttf -lsndfile -lm -g -pg -fprofile-instr-generate
LINUX_CFLAGS :=  -O2 -Wall -Wextra -pg -fprofile-instr-generate -fcoverage-mapping
LINUX_BIN := LINUX

WIN_TARGET := fftplayer.exe
WIN_CC := x86_64-w64-mingw32-gcc
WIN_LDFLAGS := -Lresources/SDL2/lib  -Lresources/libsndfile/lib  -Lresources/SDL2_TTF/lib  -Lresources/x86_64-w64-mingw32/lib -lm -g -lmingw32  -mwindows -lSDL2main -lSDL2 -lpthread -lgdi32 -lSDL2_ttf -lsndfile 
LIBINC := -Iresources/SDL2/include -Iresources/libsndfile/include -Iresources/SDL2_TTF/include -Iresources/x86_64-w64-mingw32/include
WIN_CFLAGS :=  -O2 -Wall -Wextra
WIN_BIN := WINDOWS

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS := $(INC_FLAGS) -MMD -MP

linux: CFLAGS = $(LINUX_CFLAGS)
linux: CC = $(LINUX_CC)
linux: $(BUILD_DIR)/$(LINUX_TARGET)
linux: BINARY_BIN := $(LINUX_BIN)
linux: TARGET := $(LINUX_TARGET)
linux: bin
linux: clean

windows: CFLAGS = $(WIN_CFLAGS)
windows: CC = $(WIN_CC)
windows: CPPFLAGS += $(LIBINC)
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
	cp $(FONT_DIR)/$(FONT) $(BINARY_BIN)
	mv $(BUILD_DIR)/$(TARGET) $(BINARY_BIN)

dllmove:
	cp resources/libsndfile/bin/sndfile.dll $(BINARY_BIN)
	cp resources/x86_64-w64-mingw32/lib/libwinpthread-1.dll $(BINARY_BIN)
	cp resources/SDL2/bin/SDL2.dll $(BINARY_BIN)
	cp resources/SDL2_TTF/bin/SDL2_ttf.dll $(BINARY_BIN)

clean:
	rm -r $(BUILD_DIR)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
