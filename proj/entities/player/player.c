#include "player.h"
#include "graphics.h"
#include "sprites/player_sprites.h"
#include <stddef.h>
#include <string.h>

/* Constantes padrão para o jogador */
#define PLAYER_INITIAL_X 100
#define PLAYER_INITIAL_Y 500
#define PLAYER_WIDTH 60
#define PLAYER_HEIGHT 30
#define PLAYER_SPEED 5
#define PLAYER_COLOR 0x2A

/* Constantes para bala do jogador */
#define PLAYER_BULLET_WIDTH  6
#define PLAYER_BULLET_HEIGHT 14
#define PLAYER_BULLET_SPEED  14
#define PLAYER_BULLET_COLOR  0x3F

/* Constantes de animação de blink ao ser atingido */
#define BLINK_DURATION   45   /* ticks totais da animação */
#define BLINK_INTERVAL   6    /* ticks entre cada toggle de visibilidade */

const BulletConfig PLAYER_BULLET_CONFIG = {
    PLAYER_BULLET_WIDTH,
    PLAYER_BULLET_HEIGHT,
    PLAYER_BULLET_SPEED,
    PLAYER_BULLET_COLOR,
    BULLET_DIR_UP
};

void player_fire_bullet(Bullet *bullet, const Player *player) {
    if (bullet == NULL || player == NULL) {
        return;
    }

    bullet->active = true;
    bullet->x = player->x + player->width  / 2 - bullet->width / 2;
    bullet->y = player->y - bullet->height;
    bullet->trail_count = 0;
}

int player_init(Player *player) {
    if (player == NULL) {
        return -1;
    }

    player->initial_x = PLAYER_INITIAL_X;
    player->initial_y = PLAYER_INITIAL_Y;
    player->x = PLAYER_INITIAL_X;
    player->y = PLAYER_INITIAL_Y;
    player->width = PLAYER_WIDTH;
    player->height = PLAYER_HEIGHT;
    player->speed = PLAYER_SPEED;
    player->color = PLAYER_COLOR;

    player->score = 0;
    player->lives = MAX_LIVES;
    memset(player->name, 0, sizeof(player->name));

    player->moving_left  = false;
    player->moving_right = false;

    player->is_blinking = false;
    player->blink_timer = 0;
    player->blink_show  = true;

    return 0;
}

void player_move_left(Player *player, int screen_width) {
    if (player == NULL) {
        return;
    }

    (void) screen_width;

    if (player->x >= player->speed) {
        player->x -= player->speed;
    }
}

void player_move_right(Player *player, int screen_width) {
    if (player == NULL) {
        return;
    }

    if (player->x + player->width + player->speed < screen_width) {
        player->x += player->speed;
    }
}

void player_draw(const Player *player) {
    if (player == NULL) {
        return;
    }

    graphics_draw_xpm((xpm_map_t) PLAYER_SPRITE,
                      (uint16_t) player->x, (uint16_t) player->y);
}

void player_erase(const Player *player, uint32_t background_color) {
    if (player == NULL) {
        return;
    }

    graphics_draw_rectangle(player->x, player->y,
                           player->width, player->height,
                           background_color);
}

void player_reset_position(Player *player) {
    if (player == NULL) {
        return;
    }

    player->x = player->initial_x;
    player->y = player->initial_y;
}

void player_reset_state(Player *player) {
    if (player == NULL) {
        return;
    }

    player->moving_left  = false;
    player->moving_right = false;

    player->is_blinking = false;
    player->blink_timer = 0;
    player->blink_show  = true;
}

void player_update_blinking(Player *player) {
    if (player == NULL) {
        return;
    }

    if (!player->is_blinking) {
        return;
    }

    player->blink_timer--;
    if (player->blink_timer <= 0) {
        player->is_blinking = false;
        player->blink_show  = true;
    } else {
        if (player->blink_timer % BLINK_INTERVAL == 0) {
            player->blink_show = !player->blink_show;
        }
    }
}

bool player_is_visible(const Player *player) {
    if (player == NULL) {
        return true;
    }

    return !player->is_blinking || player->blink_show;
}

void player_on_hit(Player *player) {
    if (player == NULL) {
        return;
    }

    player->lives--;
    player->is_blinking = true;
    player->blink_timer = BLINK_DURATION;
    player->blink_show  = false;
}
