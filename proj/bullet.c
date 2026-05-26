#include "bullet.h"
#include "graphics.h"
#include <stddef.h>

/* Constantes para bala do jogador */
#define BULLET_PLAYER_WIDTH 6
#define BULLET_PLAYER_HEIGHT 14
#define BULLET_PLAYER_SPEED 8
#define BULLET_PLAYER_COLOR 0x3F

/* Constantes para bala alienígena */
#define BULLET_ALIEN_WIDTH 6
#define BULLET_ALIEN_HEIGHT 14
#define BULLET_ALIEN_SPEED 5
#define BULLET_ALIEN_COLOR 0x3C

int bullet_player_init(Bullet *bullet) {
    if (bullet == NULL) {
        return -1;
    }
    
    bullet->width = BULLET_PLAYER_WIDTH;
    bullet->height = BULLET_PLAYER_HEIGHT;
    bullet->speed = BULLET_PLAYER_SPEED;
    bullet->color = BULLET_PLAYER_COLOR;
    bullet->active = false;
    bullet->x = 0;
    bullet->y = 0;
    
    return 0;
}

int bullet_alien_init(Bullet *bullet) {
    if (bullet == NULL) {
        return -1;
    }
    
    bullet->width = BULLET_ALIEN_WIDTH;
    bullet->height = BULLET_ALIEN_HEIGHT;
    bullet->speed = BULLET_ALIEN_SPEED;
    bullet->color = BULLET_ALIEN_COLOR;
    bullet->active = false;
    bullet->x = 0;
    bullet->y = 0;
    
    return 0;
}

void bullet_fire_from_player(Bullet *bullet, int player_x, int player_y, 
                             int player_width, int player_height) {
    if (bullet == NULL) {
        return;
    }
    
    bullet->active = true;
    bullet->x = player_x + player_width / 2 - bullet->width / 2;
    bullet->y = player_y - bullet->height;
}

void bullet_fire_from_alien(Bullet *bullet, int alien_x, int alien_y, 
                            int alien_width, int alien_height) {
    if (bullet == NULL) {
        return;
    }
    
    bullet->active = true;
    bullet->x = alien_x + alien_width / 2 - bullet->width / 2;
    bullet->y = alien_y + alien_height;
}

bool bullet_update_player(Bullet *bullet) {
    if (bullet == NULL || !bullet->active) {
        return false;
    }
    
    if (bullet->y >= bullet->speed) {
        bullet->y -= bullet->speed;
        return true;
    } else {
        bullet->active = false;
        return false;
    }
}

bool bullet_update_alien(Bullet *bullet, int screen_height) {
    if (bullet == NULL || !bullet->active) {
        return false;
    }
    
    if (bullet->y + bullet->height + bullet->speed < screen_height) {
        bullet->y += bullet->speed;
        return true;
    } else {
        bullet->active = false;
        return false;
    }
}

void bullet_draw(const Bullet *bullet) {
    if (bullet == NULL || !bullet->active) {
        return;
    }
    
    graphics_draw_rectangle(bullet->x, bullet->y, 
                           bullet->width, bullet->height, 
                           bullet->color);
}

void bullet_erase(const Bullet *bullet, uint32_t background_color) {
    if (bullet == NULL) {
        return;
    }
    
    graphics_draw_rectangle(bullet->x, bullet->y, 
                           bullet->width, bullet->height, 
                           background_color);
}

void bullet_deactivate(Bullet *bullet) {
    if (bullet == NULL) {
        return;
    }
    
    bullet->active = false;
}
