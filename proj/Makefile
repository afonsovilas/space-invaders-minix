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

.PATH: drivers/keyboard drivers/mouse drivers/timer \
       graphics \
       entities/player entities/alien entities/bullet entities/shield \
       game \
       ui/menu ui/scoreboard

CPPFLAGS += -I. \
            -Idrivers -Idrivers/keyboard -Idrivers/mouse -Idrivers/timer \
            -Igraphics -Igraphics/sprites \
            -Ientities -Ientities/player -Ientities/alien -Ientities/bullet -Ientities/shield \
            -Igame \
            -Iui -Iui/menu -Iui/scoreboard

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>