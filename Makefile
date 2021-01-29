NAME = webserv
CFLAGS =  -fsanitize=address -g -Wall -Wextra -Werror
SRCS = ${addprefix srcs/, ${shell ls srcs | grep '\.cpp'}}
OBJS = ${SRCS:.cpp=.o}
CC = c++
RM = rm -f
INC = -I./includes
LIB = -L./libft -lft

${NAME}: ${OBJS}
	make -C ./libft
	${CC} ${CFLAGS} ${INC} ${OBJS} ${LIB} -o ${NAME}


%.o : %.cpp
	${CC} ${CFLAGS} ${INC} -c $< -o $@

all: ${NAME}

clean:
	${RM} ${OBJS}
	make -C ./libft clean

fclean: clean
	${RM} ${NAME}
	make -C ./libft fclean

re: fclean all
