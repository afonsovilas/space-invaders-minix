#include "shield.h"
#include "graphics.h"
#include <stddef.h>

/* Constantes padrão para escudos */
#define SHIELD_WIDTH 90
#define SHIELD_HEIGHT 30
#define SHIELD_COLOR 0x02
#define SHIELD_INITIAL_Y 400
#define SHIELD_X_SPACING 220

int shield_array_init(ShieldArray *shields) {
    if (shields == NULL) {
        return -1;
    }
    
    for (int i = 0; i < SHIELD_COUNT; i++) {
        shields->shields[i].x = 120 + i * SHIELD_X_SPACING;
        shields->shields[i].y = SHIELD_INITIAL_Y;
        shields->shields[i].width = SHIELD_WIDTH;
        shields->shields[i].height = SHIELD_HEIGHT;
        shields->shields[i].color = SHIELD_COLOR;
        shields->shields[i].alive = true;
    }
    
    return 0;
}

void shield_destroy(ShieldArray *shields, int shield_index) {
    if (shields == NULL || shield_index < 0 || shield_index >= SHIELD_COUNT) {
        return;
    }
    
    shields->shields[shield_index].alive = false;
}

void shield_array_draw(const ShieldArray *shields) {
    if (shields == NULL) {
        return;
    }
    
    for (int i = 0; i < SHIELD_COUNT; i++) {
        if (shields->shields[i].alive) {
            graphics_draw_rectangle(shields->shields[i].x, shields->shields[i].y,
                                   shields->shields[i].width, shields->shields[i].height,
                                   shields->shields[i].color);
        }
    }
}

void shield_erase(const ShieldArray *shields, int shield_index, 
                  uint32_t background_color) {
    if (shields == NULL || shield_index < 0 || shield_index >= SHIELD_COUNT) {
        return;
    }
    
    graphics_draw_rectangle(shields->shields[shield_index].x, 
                           shields->shields[shield_index].y,
                           shields->shields[shield_index].width, 
                           shields->shields[shield_index].height,
                           background_color);
}
