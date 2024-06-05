
UNAME_S := $(shell uname -s)
TARGET_EXEC := fftplayer.exe

CC := x86_64-w64-mingw32-gcc
BUILD_DIR := ./build
SRC_DIRS := ./src

LDFLAGS := -Lresources/SDL2/lib  -Lresources/libsndfile/lib  -Lresources/SDL2_TTF/lib  -Lresources/x86_64-w64-mingw32/lib -lm -g -lmingw32  -mwindows -lSDL2main -lSDL2 -lpthread -lgdi32 -lSDL2_ttf -lsndfile 
EXEC_PATH := build/$(TARGET_EXEC)

LIBINC := -Iresources/SDL2/include -Iresources/libsndfile/include -Iresources/SDL2_TTF/include -Iresources/x86_64-w64-mingw32/include

CFLAGS :=  -O2 -Wall -Wextra

# Find all the C and C++ files we want to compile
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Preprocessor flags
CPPFLAGS := $(LIBINC) $(INC_FLAGS) -MMD -MP


# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) 

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


.PHONY: all clean install uninstall

all: $(BUILD_DIR)/$(TARGET_EXEC)

uninstall:

install:

clean:
	rm -r $(BUILD_DIR)
	rm -f *.profdata
	rm -f *.profraw
	rm -f perfcov.json
	rm -f gmon.out
	rm -f report.txt

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
