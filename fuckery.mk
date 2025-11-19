# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    fuckery.mk                                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/16 22:27:59 by kiroussa          #+#    #+#              #
#    Updated: 2025/11/19 02:39:28 by kiroussa         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# :trollface:

# Project types:
# 0 -> ft_shield (lvincent)
# 1 -> MattDaemon (kiroussa)

PROJECT_TYPE := 0
__DETERMINED_TYPE := 0
HAS_GIT := $(shell [ -d .git ] && echo 1 || echo 0)

ifeq ($(HAS_GIT), 1) # if we have a git remote, get it
	REMOTE_URL := $(shell git remote get-url origin)

	# are we vogsphere?
	IS_VOGSPHERE := $(shell echo $(REMOTE_URL) | grep -c vogsphere >/dev/null && echo 1 || echo 0)

	ifeq ($(IS_VOGSPHERE), 1)
		# grep the login from the remote url
		IS_KIROUSSA := $(shell echo $(REMOTE_URL) | grep -c kiroussa >/dev/null && echo 1 || echo 0)
		PROJECT_TYPE := $(IS_KIROUSSA)
		__DETERMINED_TYPE := 1
	else
		IS_NOT_SHIELD := $(shell echo $(REMOTE_URL) | grep -c shield >/dev/null && echo 0 || echo 1)
		PROJECT_TYPE := $(IS_NOT_SHIELD)
		__DETERMINED_TYPE := 1
	endif
endif

# if we don't have a vogsphere git remote
ifneq ($(__DETERMINED_TYPE), 1)
	# let's guess from the directory name
	PARENT_DIR := $(shell basename $(shell pwd -L))
	IS_NOT_SHIELD := $(shell echo $(PARENT_DIR) | grep -c shield >/dev/null && echo 0 || echo 1)
	PROJECT_TYPE := $(IS_NOT_SHIELD)
	__DETERMINED_TYPE := 1
endif

ifeq ($(PROJECT_TYPE), 0)
	NAME := ft_shield
else
	NAME := MattDaemon
endif

_ := $(info === Running GNU make for $(NAME) ===)
