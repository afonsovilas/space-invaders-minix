#include "player.h"
#include "graphics.h"
#include <stddef.h>

/* Constantes padrão para o jogador */
#define PLAYER_INITIAL_X 100
#define PLAYER_INITIAL_Y 500
#define PLAYER_WIDTH 60
#define PLAYER_HEIGHT 30
#define PLAYER_SPEED 5
#define PLAYER_COLOR 0x2A

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
    
    return 0;
}

void player_move_left(Player *player, int screen_width) {
    if (player == NULL) {
        return;
    }
    
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
    
    graphics_draw_rectangle(player->x, player->y, 
                           player->width, player->height, 
                           player->color);
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
