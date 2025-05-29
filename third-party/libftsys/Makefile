# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/10 23:59:16 by kiroussa          #+#    #+#              #
#    Updated: 2025/05/25 02:01:37 by kiroussa         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libftsys.a

SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include

-include development.mk

ifeq ($(DEVELOPMENT), 1)
_ := $(shell bash gensources.sh sources.mk $(SRC_DIR))
endif

include sources.mk

OBJS := $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(patsubst %.cpp,%.o,$(SRCS))))
SRCS := $(addprefix $(SRC_DIR)/,$(SRCS))
OBJS := $(addprefix $(OBJ_DIR)/,$(OBJS))

CC := gcc
CFLAGS := -Wall -Wextra -Werror -ffreestanding -fno-stack-protector -nostdlib -nostdinc -O2 -g3
CFLAGS += -I$(INC_DIR)

NASM := nasm
NASMFLAGS := -f elf64

CXX := g++
CXXFLAGS := -Wall -Wextra -Werror -ffreestanding -fno-stack-protector -nostdlib -nostdinc -O2 -g3
CXXFLAGS += -I$(INC_DIR)

AR := ar
ARFLAGS := rcs

all: $(NAME)

$(NAME): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(NASM) $(NASMFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

oclean:
	rm -rf $(OBJ_DIR)

clean: oclean

fclean: oclean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
