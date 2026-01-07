# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    hiding.mk                                          :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/06 22:02:25 by kiroussa          #+#    #+#              #
#    Updated: 2026/01/06 22:17:54 by kiroussa         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# This generates a random service name and description for the daemon to hide it

SERVICE_NAMES_JSON := $(MAKE_DIR)/names.json
SERVICES_SIZE := $(shell jq '. | length' $(SERVICE_NAMES_JSON))
RANDOM_INT := $(shell echo $$(( $$RANDOM % $(SERVICES_SIZE) )))
SERVICE_NAME := $(shell jq -r ".[$(RANDOM_INT)].name" $(SERVICE_NAMES_JSON))
SERVICE_DESCRIPTION := $(shell jq -r ".[$(RANDOM_INT)].description" $(SERVICE_NAMES_JSON))

# _ := $(info $(SERVICE_NAME) - $(SERVICE_DESCRIPTION))

CFLAGS += -DSHIELD_SERVICE_NAME="\"$(SERVICE_NAME)"\"
CFLAGS += -DSHIELD_SERVICE_DESCRIPTION="\"$(SERVICE_DESCRIPTION)"\"
