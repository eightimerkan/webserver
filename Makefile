NAME=webserv
CC = g++
FLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -rf

.PHONY: all clean fclean re


all: $(NAME)

$(NAME): main.cpp src/CgiHandler.cpp src/Config.cpp src/ConfigLocation.cpp src/ConfigServer.cpp src/Request.cpp src/Response.cpp src/Server.cpp src/Socket.cpp src/Utils.cpp
	@$(CC) $(FLAGS) main.cpp src/CgiHandler.cpp src/Config.cpp src/ConfigLocation.cpp src/ConfigServer.cpp src/Request.cpp src/Response.cpp src/Server.cpp src/Socket.cpp src/Utils.cpp -o $(NAME)

clean:
	@$(RM) $(NAME)

fclean: clean

re: fclean all