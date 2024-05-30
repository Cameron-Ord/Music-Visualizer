TARGET_EXEC := fftplayer
CC := clang
BUILD_DIR := ./build
SRC_DIRS := ./src
LDFLAGS := -lSDL2 -lsndfile -lSDL2_ttf -lm -g -Wall -Wextra 

CFLAGS := -pg -fprofile-instr-generate -fcoverage-mapping -O2
LDFLAGS += -pg -fprofile-instr-generate

# Find all the C and C++ files we want to compile
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Preprocessor flags
CPPFLAGS := $(INC_FLAGS) -MMD -MP

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) 

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@



.PHONY: clean
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
