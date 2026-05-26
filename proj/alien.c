#include "alien.h"
#include "graphics.h"
#include <stddef.h>

/* Constantes padrão para alienigenas */
#define ALIEN_INITIAL_X 100
#define ALIEN_INITIAL_Y 80
#define ALIEN_WIDTH 40
#define ALIEN_HEIGHT 25
#define ALIEN_X_SPACING 20
#define ALIEN_Y_SPACING 20
#define ALIEN_SPEED 2
#define ALIEN_DROP 20
#define ALIEN_COLOR 0x1C
#define ALIEN_MOVE_INTERVAL 10
#define ALIEN_SHOOT_INTERVAL 120

int alien_swarm_init(AlienSwarm *swarm) {
    if (swarm == NULL) {
        return -1;
    }
    
    swarm->speed = ALIEN_SPEED;
    swarm->direction = 1;
    swarm->drop = ALIEN_DROP;
    swarm->frame_counter = 0;
    swarm->move_interval = ALIEN_MOVE_INTERVAL;
    swarm->shoot_counter = 0;
    swarm->shoot_interval = ALIEN_SHOOT_INTERVAL;
    swarm->shoot_column = 0;
    
    /* Inicializa cada alienigena */
    for (int row = 0; row < ALIEN_ROWS; row++) {
        for (int col = 0; col < ALIEN_COLS; col++) {
            int i = row * ALIEN_COLS + col;
            
            swarm->aliens[i].x = ALIEN_INITIAL_X + col * (ALIEN_WIDTH + ALIEN_X_SPACING);
            swarm->aliens[i].y = ALIEN_INITIAL_Y + row * (ALIEN_HEIGHT + ALIEN_Y_SPACING);
            swarm->aliens[i].width = ALIEN_WIDTH;
            swarm->aliens[i].height = ALIEN_HEIGHT;
            swarm->aliens[i].color = ALIEN_COLOR;
            swarm->aliens[i].alive = true;
        }
    }
    
    return 0;
}

void alien_swarm_update(AlienSwarm *swarm) {
    if (swarm == NULL) {
        return;
    }
    
    swarm->frame_counter++;
    
    if (swarm->frame_counter >= swarm->move_interval) {
        bool should_change_direction = false;
        
        /* Verifica se algum alienigena atingiu a borda */
        for (int i = 0; i < ALIEN_COUNT; i++) {
            if (swarm->aliens[i].alive) {
                if (swarm->direction == 1 && 
                    swarm->aliens[i].x + swarm->aliens[i].width + swarm->speed >= 1024) {
                    should_change_direction = true;
                }
                
                if (swarm->direction == -1 && 
                    swarm->aliens[i].x - swarm->speed <= 0) {
                    should_change_direction = true;
                }
            }
        }
        
        if (should_change_direction) {
            swarm->direction *= -1;
            
            /* Desce todos os alienigenas */
            for (int i = 0; i < ALIEN_COUNT; i++) {
                if (swarm->aliens[i].alive) {
                    swarm->aliens[i].y += swarm->drop;
                }
            }
        } else {
            /* Move todos os alienigenas horizontalmente */
            for (int i = 0; i < ALIEN_COUNT; i++) {
                if (swarm->aliens[i].alive) {
                    swarm->aliens[i].x += swarm->speed * swarm->direction;
                }
            }
        }
        
        swarm->frame_counter = 0;
    }
}

bool alien_swarm_all_dead(const AlienSwarm *swarm) {
    if (swarm == NULL) {
        return false;
    }
    
    for (int i = 0; i < ALIEN_COUNT; i++) {
        if (swarm->aliens[i].alive) {
            return false;
        }
    }
    
    return true;
}

bool alien_swarm_reached_player(const AlienSwarm *swarm, int player_y) {
    if (swarm == NULL) {
        return false;
    }
    
    for (int i = 0; i < ALIEN_COUNT; i++) {
        if (swarm->aliens[i].alive) {
            if (swarm->aliens[i].y + swarm->aliens[i].height >= player_y) {
                return true;
            }
        }
    }
    
    return false;
}

bool alien_swarm_get_shooter(AlienSwarm *swarm, int *alien_index) {
    if (swarm == NULL || alien_index == NULL) {
        return false;
    }
    
    swarm->shoot_counter++;
    
    if (swarm->shoot_counter < swarm->shoot_interval) {
        return false;
    }
    
    /* Procura o alienigena mais baixo da coluna rotativa */
    for (int attempt = 0; attempt < ALIEN_COLS; attempt++) {
        int col = (swarm->shoot_column + attempt) % ALIEN_COLS;
        
        for (int row = ALIEN_ROWS - 1; row >= 0; row--) {
            int i = row * ALIEN_COLS + col;
            
            if (swarm->aliens[i].alive) {
                *alien_index = i;
                swarm->shoot_column = (col + 1) % ALIEN_COLS;
                swarm->shoot_counter = 0;
                return true;
            }
        }
    }
    
    swarm->shoot_counter = 0;
    return false;
}

void alien_kill(AlienSwarm *swarm, int alien_index) {
    if (swarm == NULL || alien_index < 0 || alien_index >= ALIEN_COUNT) {
        return;
    }
    
    swarm->aliens[alien_index].alive = false;
}

void alien_swarm_draw(const AlienSwarm *swarm) {
    if (swarm == NULL) {
        return;
    }
    
    for (int i = 0; i < ALIEN_COUNT; i++) {
        if (swarm->aliens[i].alive) {
            graphics_draw_rectangle(swarm->aliens[i].x, swarm->aliens[i].y,
                                   swarm->aliens[i].width, swarm->aliens[i].height,
                                   swarm->aliens[i].color);
        }
    }
}

void alien_swarm_erase(const AlienSwarm *swarm, uint32_t background_color) {
    if (swarm == NULL) {
        return;
    }
    
    for (int i = 0; i < ALIEN_COUNT; i++) {
        if (swarm->aliens[i].alive) {
            graphics_draw_rectangle(swarm->aliens[i].x, swarm->aliens[i].y,
                                   swarm->aliens[i].width, swarm->aliens[i].height,
                                   background_color);
        }
    }
}
