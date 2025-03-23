# This file was generated using ft-templates
# https://github.com/seekrs/ft-templates

NAME := ft_shield
-include development.mk

ifeq ($(DEVELOPMENT), 1)
DEBUG := 1
_ := $(shell bash gensources.sh sources.mk $(SRC_DIR))
endif

DEBUG ?= 0
BONUS ?= 0

CC := clang
CFLAGS := -Wall -Wextra -Werror
LDFLAGS :=

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

OBJS := $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(patsubst %.cpp,%.o,$(SRCS))))
SRCS := $(addprefix $(SRC_DIR)/,$(SRCS))
OBJS := $(addprefix $(OBJ_DIR)/,$(OBJS))

LIB_DIR := third-party
LIBRARIES += libft
libft_URL := https://codeberg.org/27/libft.git
libft_DIR := $(LIB_DIR)/libft
libft_DEP := $(libft_DIR)/build/output/libft.a
CFLAGS += -I$(libft_DIR)/include -I$(libft_DIR)/includes
LDFLAGS += $(libft_DEP)
DEPFILES += $(libft_DEP)
LDFLAGS += -lSDL2

all: $(NAME)

$(NAME): $(OBJS) $(DEPFILES)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(NASM) $(NASMFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(libft_DEP):
	make -C $(libft_DIR) -j$(shell nproc)

oclean:
	rm -rf $(BUILD_DIR)

clean: oclean
	make -C $(libft_DIR) clean

fclean: oclean
	make -C $(libft_DIR) fclean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean oclean fclean re
