SRC_DIRS := src
INCLUDE_DIRS := include
BUILD_DIR := build
TARGET_LIB := obj_loader
TEST_DIR := test
LIB_DIR := lib
LIBS := hashmap

LIBS := $(addprefix -l, $(LIBS))
SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

TEST_SRCS := $(shell find $(TEST_DIR) -name '*.c')

INCLUDE_FLAGS = $(addprefix -I, $(INCLUDE_DIRS))
TARGET_LIB_FILE := lib$(TARGET_LIB).a

CFLAGS := $(INCLUDE_FLAGS) -MMD -MP

.PHONY: all
all : $(BUILD_DIR)/$(TARGET_LIB_FILE) .clangd

.PHONY: test
test : $(BUILD_DIR)/$(TEST_DIR)/test
	./$(BUILD_DIR)/$(TEST_DIR)/test && valgrind ./$(BUILD_DIR)/$(TEST_DIR)/test > /dev/null

$(BUILD_DIR)/$(TEST_DIR)/test : $(TEST_SRCS)  $(BUILD_DIR)/$(TARGET_LIB_FILE)
	mkdir -p $(BUILD_DIR)/$(TEST_DIR)
	$(CC) $(INCLUDE_FLAGS) $^ -o $(BUILD_DIR)/$(TEST_DIR)/test -L$(BUILD_DIR) -l$(TARGET_LIB) -L$(LIB_DIR) $(LIBS)

$(BUILD_DIR)/$(TARGET_LIB_FILE) : $(OBJS)
	ar rcs $(BUILD_DIR)/$(TARGET_LIB_FILE) $(OBJS)

$(OBJS) : $(SRCS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.clangd :
	printf "CompileFlags:\n\t\
		Add: [-I$$PWD/$(INCLUDE_DIRS)]" > .clangd

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)

