PROG=proj

SRCS= main.c          \
      app.c           \
      utils.c         \
      keyboard.c      \
      mouse.c         \
      timer.c         \
      graphics.c      \
      font.c          \
      player.c        \
      alien.c         \
      bullet.c        \
      shield.c        \
      game.c          \
      game_state.c    \
      menu.c          \
      menu_state.c    \
      name_input.c    \
      name_input_state.c \
      scoreboard.c    \
      scoreboard_state.c \
      background.c

.PATH: app \
       drivers/utils drivers/keyboard drivers/mouse drivers/timer drivers/graphics \
       entities/player entities/alien entities/bullet entities/shield \
       game \
       ui/menu ui/scoreboard ui/graphics ui/graphics/sprites

CPPFLAGS += -I. \
            -Iapp \
            -Idrivers -Idrivers/utils \
            -Idrivers/keyboard -Idrivers/mouse -Idrivers/timer -Idrivers/graphics \
            -Ientities -Ientities/player -Ientities/alien -Ientities/bullet -Ientities/shield \
            -Igame \
            -Iui -Iui/menu -Iui/scoreboard -Iui/graphics -Iui/graphics/sprites

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
