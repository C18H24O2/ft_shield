# This file was generated using ft-templates
# https://github.com/seekrs/ft-templates

NAME := libshitass-poopface.so
MAKE := make --no-print-directory --debug=none
-include development.mk

ifeq ($(DEVELOPMENT), 1)
DEBUG := 1
USE_WARNINGS := 1
_ := $(shell bash gensources.sh sources.mk $(SRC_DIR))
endif

DEBUG ?= 0

CC := clang
CXX := clang++
CFLAGS := -Wall -Wextra
ifneq ($(USE_WARNINGS), 1)
CFLAGS += -Werror
endif
CXXFLAGS := $(CFLAGS)
LDFLAGS := -shared -fPIC

NASM := nasm
NASMFLAGS := -f elf64

SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

ifeq ($(DEBUG), 1)
CFLAGS += -g3 -gdwarf-3 -ggdb
NASMFLAGS += -g
endif

include sources.mk

OBJS := $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(patsubst %.s,%.o,$(patsubst %.cpp,%.o,$(SRCS)))))
SRCS := $(addprefix $(SRC_DIR)/,$(SRCS))
OBJS := $(addprefix $(OBJ_DIR)/,$(OBJS))

LIB_DIR := third-party

all:
	@$(MAKE) -j$(shell nproc) $(NAME)

$(NAME): $(OBJS) $(DEPFILES)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(NASM) $(NASMFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S
	@mkdir -p $(dir $@)
	$(NASM) $(NASMFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

oclean:
	rm -rf $(BUILD_DIR)

clean: oclean

fclean: oclean
	rm -rf $(NAME)

compile_commands.json: oclean
	bear -- $(MAKE) USE_WARNINGS=1 $(OBJS) 

re: fclean
	@$(MAKE) -j$(shell nproc) $(NAME)

.PHONY: all clean oclean fclean re
