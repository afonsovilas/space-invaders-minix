#include "bullet.h"
#include "graphics.h"
#include "sprites/bullet_sprites.h"
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
    bullet->trail_count = 0;

    return 0;
}

bool bullet_update(Bullet *bullet, int screen_height) {
    if (bullet == NULL || !bullet->active) {
        return false;
    }

    /* Shift trail history */
    for (int i = BULLET_TRAIL_LENGTH - 1; i > 0; i--) {
        bullet->trail_x[i] = bullet->trail_x[i-1];
        bullet->trail_y[i] = bullet->trail_y[i-1];
    }
    bullet->trail_x[0] = bullet->x;
    bullet->trail_y[0] = bullet->y;

    if (bullet->trail_count < BULLET_TRAIL_LENGTH) {
        bullet->trail_count++;
    }

    bullet->y += (int) bullet->direction * bullet->speed;

    if (bullet->y < 0 || bullet->y + bullet->height > screen_height) {
        bullet->active = false;
        return false;
    }

    return true;
}

/**
 * Compute the bounding rectangle of the i-th trail particle for a bullet.
 * Shared between bullet_draw() and bullet_erase() to avoid duplicating the
 * geometry logic.
 */
static void bullet_trail_rect(const Bullet *bullet, int i,
                               int *tx, int *ty, int *tw, int *th) {
    if (bullet->direction == BULLET_DIR_UP) {
        *tw = 2;
        *th = bullet->height - (i + 1) * 4;
        if (*th < 2) *th = 2;
        *tx = bullet->trail_x[i] + (bullet->width - *tw) / 2;
        *ty = bullet->trail_y[i] + (i + 1) * 2;
    } else {
        *tw = bullet->width - (i + 1);
        *th = bullet->height - (i + 1) * 3;
        if (*tw < 1) *tw = 1;
        if (*th < 1) *th = 1;
        *tx = bullet->trail_x[i] + (bullet->width - *tw) / 2;
        *ty = bullet->trail_y[i] - (i + 1) * 2;
    }
}

void bullet_draw(const Bullet *bullet) {
    if (bullet == NULL || !bullet->active) {
        return;
    }

    /* Draw the main bullet (Player: simple rectangle, Alien: XPM sprite) */
    if (bullet->direction == BULLET_DIR_UP) {
        graphics_draw_rectangle(bullet->x, bullet->y, bullet->width, bullet->height, bullet->color);
    } else {
        graphics_draw_xpm((xpm_map_t) BULLET_SPRITE,
                          (uint16_t) bullet->x, (uint16_t) bullet->y);
    }

    /* Draw the trail particles behind it */
    int max_trail = (bullet->direction == BULLET_DIR_UP) ? 2 : bullet->trail_count;
    for (int i = 0; i < max_trail && i < bullet->trail_count; i++) {
        int tx, ty, tw, th;
        bullet_trail_rect(bullet, i, &tx, &ty, &tw, &th);

        uint32_t tcolor;
        if (bullet->direction == BULLET_DIR_UP) {
            tcolor = (i == 0) ? 0x07u : 0x08u; /* Fading greys */
        } else {
            if (i == 0)      tcolor = 0x0Eu; /* Faded yellow  */
            else if (i == 1) tcolor = 0x06u; /* Brown/orange  */
            else             tcolor = 0x14u; /* Dark brown    */
        }

        graphics_draw_rectangle(tx, ty, tw, th, tcolor);
    }
}

void bullet_erase(const Bullet *bullet, uint32_t background_color) {
    if (bullet == NULL) {
        return;
    }

    /* Erase main bullet */
    graphics_draw_rectangle(bullet->x, bullet->y,
                            bullet->width, bullet->height,
                            background_color);

    /* Erase trail particles */
    int max_trail = (bullet->direction == BULLET_DIR_UP) ? 2 : bullet->trail_count;
    for (int i = 0; i < max_trail && i < bullet->trail_count; i++) {
        int tx, ty, tw, th;
        bullet_trail_rect(bullet, i, &tx, &ty, &tw, &th);
        graphics_draw_rectangle(tx, ty, tw, th, background_color);
    }
}

void bullet_deactivate(Bullet *bullet) {
    if (bullet == NULL) {
        return;
    }
    
    bullet->active = false;
}
