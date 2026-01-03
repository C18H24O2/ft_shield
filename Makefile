# This file was generated using ft-templates
# https://github.com/seekrs/ft-templates

PASSWORD ?= Password123

include fuckery.mk

-include development.mk

ifeq ($(DEVELOPMENT), 1)
DEBUG := 1
USE_WARNINGS := 1
_ := $(shell bash gensources.sh sources.mk $(SRC_DIR))
endif

USE_LIBFTSYS := 0
USE_LIBSP := 0
USE_WW := 0

DEBUG ?= 0
BONUS ?= 0

_ := $(shell bash -c 'if [ ! -f hasher3000 ]; then cc -msse4.2 -mrdrnd -DHASH_MAIN=1 -O3 -o hasher3000 src/hash.cc; fi')

ifeq ($(PROJECT_TYPE), 1)
CC := clang++
else
CC := clang++
endif
CFLAGS := -Wall -fcolor-diagnostics -Wextra -msse4.2 -mrdrnd -Wno-unused-command-line-argument -DSHIELD_PASSWORD=\"$(shell ./hasher3000 $(PASSWORD))\"
CFLAGS += 
ifneq ($(USE_WARNINGS), 1)
CFLAGS += -Werror
endif
SHIELD_PORT ?= 4242
CFLAGS += -DMATT_MODE=$(PROJECT_TYPE) -DFT_SHIELD_PORT=$(SHIELD_PORT) -DFT_SHIELD_PORT_STRING=\"$(SHIELD_PORT)\"

LD := clang++
LDFLAGS := -lX11

ifeq ($(USE_LIBFTSYS), 1)
CFLAGS += -nostdlib -nostdinc -ffreestanding
LDFLAGS += -nostdlib -nostartfiles -ffreestanding
endif

CXX := clang++
CXXFLAGS := $(CFLAGS)

NASM := nasm
NASMFLAGS := -f elf64

SRC_DIR := src
BUILD_DIR := build
INC_DIR := include
OBJ_DIR := $(BUILD_DIR)/obj

#TODO: maybe move all .c to .cc or .cpp files
CFLAGS += -I$(INC_DIR)
CXXFLAGS += -I$(INC_DIR)

CFLAGS += $(EXTRA_CFLAGS)
CXXFLAGS += $(EXTRA_CXXFLAGS)

ifeq ($(DEBUG), 1)
CFLAGS += -g3 -ggdb -gdwarf-3 -DSHIELD_DEBUG=1
CXXFLAGS += -g3 -ggdb -gdwarf-3 -DSHIELD_DEBUG=1
NASMFLAGS += -g
endif

include sources.mk

CXXFLAGS += $(SRC_DIRS:%=-I%)

LIB_DIR := third-party

LIBFTSYS_DIR := $(LIB_DIR)/libftsys
LIBFTSYS := $(LIBFTSYS_DIR)/libftsys.a
LIBFTSTD_DIR := $(LIB_DIR)/libftstd
LIBFTSTD := $(LIBFTSTD_DIR)/libftstd.a
LIBSP_DIR := $(LIB_DIR)/shitass-poopface
LIBSP := $(LIBSP_DIR)/libshitass-poopface.so
WW_DIR := $(LIB_DIR)/woody-woodpacker
WW_BIN := $(WW_DIR)/woody_woodpacker

ifeq ($(USE_LIBFTSYS), 1)
CFLAGS += -I$(LIBFTSYS_DIR)/include -I$(LIBFTSTD_DIR)/include
CXXFLAGS += -I$(LIBFTSYS_DIR)/include -I$(LIBFTSTD_DIR)/include
endif

OBJS := $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(SRCS)))))
SRCS := $(addprefix $(SRC_DIR)/,$(SRCS))
OBJS := $(addprefix $(OBJ_DIR)/,$(OBJS))

ifeq ($(USE_LIBSP), 1)
CFLAGS += -fpass-plugin=$(LIBSP)
MAIN_DEPS += $(LIBSP)
endif
MAIN_DEPS += $(OBJS)
LINK_DEPS += $(OBJS)
ifeq ($(USE_LIBFTSYS), 1)
MAIN_DEPS += $(LIBFTSYS) $(LIBFTSTD)
LINK_DEPS += $(LIBFTSYS) $(LIBFTSTD)
endif
ifeq ($(USE_WW), 1)
MAIN_DEPS += $(WW_BIN)
endif

all: $(NAME)

ft_shield MattDaemon: $(MAIN_DEPS) 
	$(LD) $(LDFLAGS) -o $@ $(LINK_DEPS) 
ifeq ($(DEBUG), 0)
	strip -xXs $@
else
ifeq ($(USE_WW), 1)
	mv $@ $@.tmp
	rm -f $@
	$(WW_BIN) -p none -o $@ $@.tmp
	rm -f $@.tmp
endif
endif

matt-daemon:
	$(MAKE) PROJECT_TYPE=1 MattDaemon

kill-daemon:
	kill -9 $(shell ps aux | grep ft_shield | grep -v grep | xargs echo | cut -d' ' -f2) 2>/dev/null || true
	kill -9 $(shell ps aux | grep MattDaemon | grep -v grep | xargs echo | cut -d' ' -f2) 2>/dev/null || true

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(NASM) $(NASMFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFTSYS): $(LIBFTSYS_DIR)/Makefile
	$(MAKE) -C $(LIBFTSYS_DIR) -j$(shell nproc)

$(LIBFTSTD): $(LIBFTSTD_DIR)/Makefile
	$(MAKE) -C $(LIBFTSTD_DIR) -j$(shell nproc) LIBFTSYS_DIR=../libftsys

$(LIBSP): $(LIBSP_DIR)/Makefile
	$(MAKE) -C $(LIBSP_DIR) -j$(shell nproc)

$(WW_BIN):
	$(MAKE) -C $(WW_DIR) -j$(shell nproc) PEDANTIC=0 CC="$(CC)"

oclean:
	rm -rf $(BUILD_DIR)

clean: oclean
ifeq ($(USE_LIBFTSYS), 1)
	$(MAKE) -C $(LIBFTSTD_DIR) clean LIBFTSYS_DIR=../libftsys
endif
ifeq ($(USE_LIBSP), 1)
	$(MAKE) -C $(LIBSP_DIR) clean
endif
ifeq ($(USE_WW), 1)
	$(MAKE) -C $(WW_DIR) clean
endif

fclean: oclean
ifeq ($(USE_LIBFTSYS), 1)
	$(MAKE) -C $(LIBFTSTD_DIR) fclean LIBFTSYS_DIR=../libftsys
endif
ifeq ($(USE_LIBSP), 1)
	$(MAKE) -C $(LIBSP_DIR) fclean
endif
ifeq ($(USE_WW), 1)
	$(MAKE) -C $(WW_DIR) fclean
endif
	rm -rf ft_shield MattDaemon 

re: fclean all

vm:
	vagrant up
	vagrant ssh

print-type:
	@echo TYPE: $(PROJECT_TYPE)
	@echo NAME: $(NAME)

copy-target: $(NAME)
	cp $(NAME) $(TO)

compile_commands.json: oclean
	bear -- $(MAKE) USE_WARNINGS=1 -j $(OBJS) 

.PHONY: all clean fclean re
