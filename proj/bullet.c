#include "bullet.h"
#include "graphics.h"
#include <stddef.h>

/* Constantes para bala do jogador */
#define BULLET_PLAYER_WIDTH 6
#define BULLET_PLAYER_HEIGHT 14
#define BULLET_PLAYER_SPEED 14
#define BULLET_PLAYER_COLOR 0x3F

int bullet_init(Bullet *bullet, const BulletConfig *config) {
    if (bullet == NULL || config == NULL) {
        return -1;
    }

    bullet->width     = config->width;
    bullet->height    = config->height;
    bullet->speed     = config->speed;
    bullet->color     = config->color;
    bullet->direction = config->direction;
    bullet->active    = false;
    bullet->x         = 0;
    bullet->y         = 0;

    return 0;
}

bool bullet_update(Bullet *bullet, int screen_height) {
    if (bullet == NULL || !bullet->active) {
        return false;
    }

    bullet->y += (int) bullet->direction * bullet->speed;

    if (bullet->y < 0 || bullet->y + bullet->height > screen_height) {
        bullet->active = false;
        return false;
    }

    return true;
}

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

void bullet_fire_from_player(Bullet *bullet, int player_x, int player_y, 
                             int player_width, int player_height) {
    if (bullet == NULL) {
        return;
    }
    
    bullet->active = true;
    bullet->x = player_x + player_width / 2 - bullet->width / 2;
    bullet->y = player_y - bullet->height;
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
