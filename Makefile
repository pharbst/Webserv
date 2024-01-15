# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/17 12:55:54 by pharbst           #+#    #+#              #
#    Updated: 2024/01/15 14:29:08 by pharbst          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

include color.mk

ifeq ($(UNAME), Darwin)
PRONAME = Webserv
else ifeq ($(UNAME), Linux)
PRONAME = Webserv_linux
endif

CC		= c++

# -MMD and -MP are ussed to create dependecy files
CFLAGS	= -Wall -Wextra -Werror -MMD -MP -g -std=c++98 $(INC_DIR)

INC_DIR	= -I./include/ -I./include/socketManager/ -I./include/Interface/

# add source files with header with the same name
SOURCE	=	socketManager.cpp \
			Interface.cpp

HEADER	= $(addprefix $(INC_DIR), $(SOURCE:.cpp=.hpp))

# add other header files here
HEADER	+= 

# add source files without header with the same name and the file with the main function has to be the first in the list
SRCS	=	webserver.cpp \
			socketManagerTools.cpp \
			InterfaceTools.cpp \
			$(SOURCE)

OBJ_DIR	= ./obj/
ifeq ($(UNAME), Darwin)
OBJ_DIR = ./obj/mac/
else ifeq ($(UNAME), Linux)
OBJ_DIR = ./obj/linux/
endif

OBJS = $(addprefix $(OBJ_DIR), $(SRCS:.cpp=.o))


# in case of subdirectories in the src folder add them here
VPATH := src include src/socketManager src/Interface

all:
	@$(MAKE) -s proname_header
	@$(MAKE) -s std_all

std_all:
	@printf "%s$(RESET)\n" "$(FPurple)Compiling $(PRONAME)"
	@-include $(OBJS:.o=.d)
	@$(MAKE) -s $(PRONAME)
	@printf "$(SETCURUP)$(CLEARLINE)$(SETCURUP)$(CLEARLINE)\r$(FPurple)%-21s$(FGreen)$(TICKBOX)$(RESET)\n" "Compiling $(PRONAME)"

$(PRONAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PRONAME)

$(OBJ_DIR)%.o: %.cpp
ifeq ($(shell test -d $(OBJ_DIR) || echo $$?), 1)
	printf "$(CLEARLINE)\r$(Yellow)creting obj dir$(RESET)"
	@mkdir -p $(OBJ_DIR)
endif
	@printf "$(CLEARLINE)\r%-28s$(RESET)" "$(Yellow)Compiling $< ..."
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(MAKE) -s proname_header
	@printf "%-28s$(RESET)" "$(FRed)Cleaning $(PRONAME)"
	@$(MAKE) -s std_clean
	@printf "$(FGreen)$(TICKBOX)$(RESET)\n"

std_clean:
	@rm -rf $(OBJ_DIR)

fclean:
	@$(MAKE) -s proname_header
	@$(MAKE) -s cleanator

cleanator:
	@printf "%-28s$(RESET)" "$(FRed)FCleaning $(PRONAME)"
	@rm -rf $(OBJ_DIR)
	@rm -f $(PRONAME)
	@printf "$(FGreen)$(TICKBOX)$(RESET)\n"

re:
	@$(MAKE) -s proname_header
	@$(MAKE) -s cleanator
	@$(MAKE) -s std_all

proname_header:
	@printf "$(FYellow)╔══════════════════════╗\n\
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