PROG=proj

SRCS= main.c        \
      keyboard.c    \
      mouse.c       \
      timer.c       \
      graphics.c    \
      font.c        \
      player.c      \
      alien.c       \
      bullet.c      \
      shield.c      \
      game.c        \
      menu.c        \
      scoreboard.c

.PATH: drivers graphics entities game ui

CPPFLAGS += -I. -Idrivers -Igraphics -Ientities -Igame -Iui

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>