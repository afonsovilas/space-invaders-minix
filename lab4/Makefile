# name of the program (Minix service)
PROG=lab4

# source code files to be compiled
SRCS = lab4.c mouse.c utils.c

# additional compilation flags
CFLAGS += -pedantic -DLAB4

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
