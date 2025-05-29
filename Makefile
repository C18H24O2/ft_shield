# This file was generated using ft-templates
# https://github.com/seekrs/ft-templates

NAME := ft_shield
-include development.mk

ifeq ($(DEVELOPMENT), 1)
DEBUG := 1
_ := $(shell bash gensources.sh sources.mk $(SRC_DIR))
endif

USE_LIBFTSYS := 0

DEBUG ?= 0
BONUS ?= 0

CC := gcc 
CFLAGS := -Wall -Wextra -Werror
LDFLAGS :=

ifeq ($(USE_LIBFTSYS), 1)
CFLAGS += -nostdlib -nostdinc -ffreestanding
LDFLAGS += -nostdlib -nostartfiles -ffreestanding
endif

CXX := g++
CXXFLAGS := $(CFLAGS)

NASM := nasm
NASMFLAGS := -f elf64

SRC_DIR := src
BUILD_DIR := build
INC_DIR := include
OBJ_DIR := $(BUILD_DIR)/obj

CFLAGS += -I$(INC_DIR)
CXXFLAGS += -I$(INC_DIR)

CFLAGS += $(EXTRA_CFLAGS)
CXXFLAGS += $(EXTRA_CXXFLAGS)

ifeq ($(DEBUG), 1)
CFLAGS += -g3 -gdwarf-3 -DSHIELD_DEBUG=1
CXXFLAGS += -g3 -gdwarf-3 -DSHIELD_DEBUG=1
NASMFLAGS += -g
endif

include sources.mk

CXXFLAGS += $(SRC_DIRS:%=-I%)

LIB_DIR := third-party

LIBFTSYS_DIR := $(LIB_DIR)/libftsys
LIBFTSYS := $(LIBFTSYS_DIR)/libftsys.a
LIBFTSTD_DIR := $(LIB_DIR)/libftstd
LIBFTSTD := $(LIBFTSTD_DIR)/libftstd.a

ifeq ($(USE_LIBFTSYS), 1)
CFLAGS += -I$(LIBFTSYS_DIR)/include -I$(LIBFTSTD_DIR)/include
CXXFLAGS += -I$(LIBFTSYS_DIR)/include -I$(LIBFTSTD_DIR)/include
endif

OBJS := $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(patsubst %.cpp,%.o,$(SRCS))))
SRCS := $(addprefix $(SRC_DIR)/,$(SRCS))
OBJS := $(addprefix $(OBJ_DIR)/,$(OBJS))

all: $(NAME)

ifeq ($(USE_LIBFTSYS), 1)
$(NAME): $(OBJS) $(LIBFTSYS) $(LIBFTSTD)
else
$(NAME): $(OBJS)
endif
	$(CC) $(LDFLAGS) -o $@ $^
ifeq ($(DEBUG), 0)
	strip $@ -s
endif

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(NASM) $(NASMFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIBFTSYS): $(LIBFTSYS_DIR)/Makefile
	$(MAKE) -C $(LIBFTSYS_DIR) -j$(shell nproc)

$(LIBFTSTD): $(LIBFTSTD_DIR)/Makefile
	$(MAKE) -C $(LIBFTSTD_DIR) -j$(shell nproc) LIBFTSYS_DIR=../libftsys

oclean:
	rm -rf $(BUILD_DIR)

clean: oclean
	$(MAKE) -C $(LIBFTSTD_DIR) clean LIBFTSYS_DIR=../libftsys

fclean: oclean
	$(MAKE) -C $(LIBFTSTD_DIR) fclean LIBFTSYS_DIR=../libftsys
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
