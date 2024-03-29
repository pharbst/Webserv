# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/17 12:55:54 by pharbst           #+#    #+#              #
#    Updated: 2024/03/26 12:00:12 by pharbst          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# openssl req -new -newkey rsa:2048 -nodes -keyout test.key -subj "/C=DE/ST=Baden-Württemberg /L=Heilbronn /O=42/CN=42heilbronn.com" -x509 -days 365 -out test.crt && cat test.key test.crt >> test.pem
# openssl req -new -newkey rsa:2048 -nodes -keyout test2.key -subj "/C=DE/ST=Baden-Württemberg /L=Heilbronn /O=42/CN=42heilbronn.com" -x509 -days 365 -out test2.crt && cat test2.key test2.crt >> test2.pem

include color.mk

ifeq ($(UNAME), Darwin)
PRONAME = Webserv
else ifeq ($(UNAME), Linux)
PRONAME = Webserv_linux
endif

# Submodules
SOCKETMANAGER_DIR	:= ./socketManager
SOCKETMANAGER		:= $(SOCKETMANAGER_DIR)/libsocketManager.a

INC_DIR		:= 	-I./include/ \
				-I./include/httpTransfer/ \
				-I$(SOCKETMANAGER_DIR)/include \
				# -I./include/config/ \
				# -I./include/error/ \
				# -I./include/httpTransfer/ \

ifeq ($(UNAME), Darwin)
SUDO		:= 
CFLAGS		:= -Wall -Wextra -Werror -MMD -MP -g -std=c++98 $(INC_DIR)
LDFLAGS		:= -L$(SOCKETMANAGER_DIR) -lsocketManager
else ifeq ($(UNAME), Linux)
SUDO		:= sudo
CFLAGS		:= -Wall -Wextra -Werror -MMD -MP -g $(INC_DIR)
LDFLAGS		:= -lssl -lcrypto -L./$(SOCKETMANAGER_DIR) -lsocketManager
endif

CC		= c++

# add source files with header with the same name
SOURCE	=	# Config.cpp \
			# Error.cpp \
			# httpTransfer.cpp

HEADER	= $(addprefix $(INC_DIR), $(SOURCE:.cpp=.hpp))

# add other header files here
HEADER	+= 

# add source files without header with the same name and the file with the main function has to be the first in the list
SRCS	=	webserver.cpp \
			interface.cpp \
			interfaceTools.cpp \
			http.cpp \
			Request.cpp \
			Response.cpp \
			Status.cpp \
			Delete.cpp \
			Get.cpp \
			Post.cpp \
			Cookies.cpp \
			Cgi.cpp \
			read.cpp \
			configFile.cpp \
			configUtil.cpp \
			location.cpp \
			mimeTypes.cpp \
			root.cpp \
			$(SOURCE)

OBJ_DIR	= ./obj/
ifeq ($(UNAME), Darwin)
OBJ_DIR = ./obj/mac/
else ifeq ($(UNAME), Linux)
OBJ_DIR = ./obj/linux/
endif

OBJS = $(addprefix $(OBJ_DIR), $(SRCS:.cpp=.o))


# in case of subdirectories in the src folder add them here
VPATH := src include src/config src/error src/httpTransfer src/parse src/interface

all:
	@$(MAKE) -s proname_header
ifeq ($(UNAME), Linux)
ifeq (($(call GET_OS,Debian)), Debian)
	@echo "$(FYellow)Information: $(Red)The Makefiles in this project are not build to work with echo as printing function so the output wont be fromated correctly$(RESET)"
endif
endif
ifeq ($(shell test -f $(SOCKETMANAGER_DIR)/Makefile || echo $$?), 1)
	@$(MAKE) $(SOCKETMANAGER)
else
	@$(MAKE) -j6 -s std_all -C socketManager
endif
	@$(MAKE) -j6 -s std_all

std_all: $(SOCKETMANAGER)
	@$(PRINT) "$(FPurple)%-40s\n$(RESET)" "Compiling $(PRONAME)"
	@-include $(OBJS:.o=.d) > /dev/null 2>&1
	@$(MAKE) -s $(PRONAME)
	@$(PRINT) "\n$(SETCURUP)$(SETCURUP)$(FPurple)%-33s$(FGreen)$(TICKBOX)$(RESET)\n$(CLEARLINE)" "Compiling $(PRONAME)"

$(PRONAME): $(OBJS) $(SOCKETMANAGER)
ifeq ($(UNAME), Darwin)
	$(CC) $(CFLAGS) -I$(shell brew --prefix)/opt/openssl@3/include/ $(OBJS) $(LDFLAGS) -L$(shell brew --prefix)/opt/openssl@3/lib/ -lssl -lcrypto -o $(PRONAME)
else ifeq ($(UNAME), Linux)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(PRONAME)
endif

$(OBJ_DIR)%.o: %.cpp
ifeq ($(shell test -d $(OBJ_DIR) || echo $$?), 1)
	@$(PRINT) "$(CLEARLINE)$(CLEARLINE)\r$(Yellow)creting obj dir$(RESET)"
	@mkdir -p $(OBJ_DIR)
endif
	@$(PRINT) "\r$(CLEARLINE)%-40s$(RESET)" "$(Yellow)Compiling $< ..."
ifeq ($(UNAME), Darwin)
	@$(CC) $(CFLAGS) -I$(shell brew --prefix)/opt/openssl@3/include/ -c $< -o $@
else ifeq ($(UNAME), Linux)
	@$(CC) $(CFLAGS) -c $< -o $@
endif

$(SOCKETMANAGER):
ifeq ($(shell test -f $(SOCKETMANAGER_DIR)/Makefile || echo $$?), 1)
	@$(PRINT) "$(CLEARLINE)$(CLEARLINE)%-40s$(RESET)" "$(FCyan)Initializing submodule"
	@git submodule update --init > /dev/null 2>&1
	@$(PRINT) "$(FGreen)$(TICKBOX)$(RESET)\n"
endif
	@$(PRINT) "$(FCyan)%-40s\n$(RESET)" "Compiling submodule"
	@$(MAKE) std_all -s -C $(SOCKETMANAGER_DIR)
ifeq ($(UNAME), Darwin)
	@$(PRINT) "$(SETCURUP)"
endif
	@$(PRINT) "$(SETCURUP)$(SETCURUP)$(SETCURUP)$(FCyan)%-33s$(RESET)$(FGreen)$(TICKBOX)$(RESET)\n\n\n" "Compiling submodule"
ifeq ($(UNAME), Darwin)
	@$(PRINT) "\n"
endif


clean:
	@$(MAKE) -s proname_header
ifeq ($(UNAME), Linux)
ifeq ($(call GET_OS,Debian), Debian)
	@echo "$(FYellow)Information: $(Red)The Makefiles in this project are not build to work with echo as printing function so the output wont be fromated correctly$(RESET)"
endif
endif
	@$(PRINT) "%-40s$(RESET)" "$(FRed)Cleaning $(PRONAME)"
	@$(MAKE) -s std_clean
	@$(PRINT) "$(FGreen)$(TICKBOX)$(RESET)\n"

fclean:
	@$(MAKE) -s proname_header
ifeq ($(UNAME), Linux)
ifeq ($(call GET_OS,Debian), Debian)
	@echo "$(FYellow)Information: $(Red)The Makefiles in this project are not build to work with echo as printing function so the output wont be fromated correctly$(RESET)"
endif
endif
	@$(MAKE) -s cleanator

re:
	@$(MAKE) -s proname_header
ifeq ($(UNAME), Linux)
ifeq ($(call GET_OS,Debian), Debian)
	@echo "$(FYellow)Information: $(Red)The Makefiles in this project are not build to work with echo"
	@echo "$(Red)             as printing function so the output wont be fromated correctly$(RESET)"
	@echo
	@echo "$(FYellow)Warning: $(Red)Unfortunately Debian has problems with the openssl library if someone knows how to fix it let me know$(RESET)"
endif
endif
	@$(MAKE) -s cleanator
	@$(MAKE) -j6 -s std_all

run: re
	./$(PRONAME)

arch:
	-docker rm -f webserv
	docker-compose -f container/Arch/docker-compose.yml build
	docker-compose -f container/Arch/docker-compose.yml up

ubuntu:
	-docker rm -f webserv
	docker-compose -f container/Ubuntu/docker-compose.yml build
	docker-compose -f container/Ubuntu/docker-compose.yml up

debian:
	-docker rm -f webserv
	docker-compose -f container/Debian/docker-compose.yml build
	docker-compose -f container/Debian/docker-compose.yml up

alpine:
	-docker rm -f webserv
	docker-compose -f ./container/Alpine/docker-compose.yml build
	docker-compose -f ./container/Alpine/docker-compose.yml up

logs:
	docker logs webserv

restart_docker:
	docker restart webserv

siege:
	brew install siege

std_clean:
	@rm -rf $(OBJ_DIR)

cleanator:
	@$(PRINT) "$(FRed)%-s$(RESET)\n" "FCleaning $(PRONAME)"
	@rm -rf $(OBJ_DIR)
	@rm -f $(PRONAME)
	@$(MAKE) cleanator -s -C socketManager
	@$(PRINT) "$(SETCURUP)$(SETCURUP)$(SETCURSTART)$(FRed)%-33s$(FGreen)$(TICKBOX)$(RESET)\n\n" "FCleaning $(PRONAME)"

proname_header:
	@$(PRINT) "$(FYellow)╔══════════════════════╗\n\
$(FYellow)║$(FRed)          (    (      $(FYellow)║$(RESET)\n\
$(FYellow)║$(FRed)     (    )\\ ) )\\ )   $(FYellow)║$(RESET)\n\
$(FYellow)║$(FRed)     )\\  (()/((()/(   $(FYellow)║$(RESET)\n\
$(FYellow)║$(FRed)   (((_)  /(_))/(_))  $(FYellow)║$(RESET)\n\
$(FYellow)║$(FRed)   )\\$(FBlue)___ $(FRed)($(FBlue)_$(FRed))) ($(FBlue)_$(FRed)))    $(FYellow)║$(RESET)\n\
$(FYellow)║$(FRed)  (($(FBlue)/ __|| _ \\| _ \\   $(FYellow)║$(RESET)\n\
$(FYellow)║$(FBlue)   | ($(FBlue)__ |  _/|  _/   $(FYellow)║$(RESET)\n\
$(FYellow)║$(FBlue)    \\___||_|  |_|     $(FYellow)║$(RESET)\n\
$(FYellow)╚══════════════════════╝\n$(RESET)"

.PHONY: all clean fclean re pro_header std_all std_clean cleanator proname_header