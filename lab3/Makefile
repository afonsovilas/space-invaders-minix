# name of the program (Minix service)
PROG=lab3

# source code files to be compiled
SRCS = lab3.c kbc.c utils.c

# additional compilation flags
CFLAGS += -pedantic -DLAB3 -I../lab2

DPADD += ${LIBLCF}
LDADD += -llcf -L../lab2
LDADD += -ltimer

.include <minix.lcom.mk>
