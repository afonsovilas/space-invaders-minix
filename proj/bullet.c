#include "bullet.h"
#include "graphics.h"
#include <stddef.h>

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
