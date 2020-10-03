NAME = webserv
CFLAGS = -Wall -Wextra -Werror
SRCS = ${addprefix srcs/, ${shell ls srcs | grep '\.cpp'}}}
OBJS = ${SRCS:.c=.o}
CC = clang++
RM = rm -f
INC = -I./includes

${NAME}: ${OBJS}
	${CC} ${CFLAGS} ${INC} ${OBJS} -o ${NAME}

%.o : %.cpp
	${CC} ${CFLAGS} ${INC} -c $< -o $@ -g

all: ${NAME}

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}

re: fclean all