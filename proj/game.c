#include "game.h"
#include "graphics.h"
#include "keyboard.h"
#include <stddef.h>

#define BACKGROUND_COLOR 0x00
#define LIVES_COLOR 0x0C      
#define BLINK_DURATION 45      
#define BLINK_INTERVAL 6       

#define LIVES_START_X 20
#define LIVES_START_Y 20
#define LIVES_WIDTH 20
#define LIVES_HEIGHT 10
#define LIVES_SPACING 30
#define LIVES_CLEAR_WIDTH (MAX_LIVES * LIVES_SPACING)

/**
 * Função auxiliar para verificar colisão entre dois retângulos.
 */

static bool check_collision(int x1, int y1, int w1, int h1,
                            int x2, int y2, int w2, int h2) {
    return x1 < x2 + w2 &&
           x1 + w1 > x2 &&
           y1 < y2 + h2 &&
           y1 + h1 > y2;
}

int game_init(Game *game, int screen_width, int screen_height) {
    if (game == NULL) {
        return -1;
    }
    
    game->state = GAME_STATE_MENU;
    game->screen_width = screen_width;
    game->screen_height = screen_height;
    game->background_color = BACKGROUND_COLOR;
    
    game->score = 0;
    game->lives = MAX_LIVES;
    
    game->moving_left = false;
    game->moving_right = false;
    
    game->is_blinking = false;
    game->blink_timer = 0;
    game->blink_show = true;
    
    if (player_init(&game->player) != 0) {
        return -1;
    }
    
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (bullet_player_init(&game->player_bullets[i]) != 0) {
            return -1;
        }
    }
    
    if (bullet_alien_init(&game->alien_bullet) != 0) {
        return -1;
    }
    
    if (alien_swarm_init(&game->alien_swarm) != 0) {
        return -1;
    }
    
    if (shield_array_init(&game->shields) != 0) {
        return -1;
    }
    
    return 0;
}

void game_start(Game *game) {
    if (game == NULL || game->state != GAME_STATE_MENU) {
        return;
    }
    
    game->state = GAME_STATE_PLAYING;
}

/* --- Funções Auxiliares de Atualização de Frame (SRP) --- */

static void game_update_blinking(Game *game) {
    if (game->is_blinking) {
        game->blink_timer--;
        if (game->blink_timer <= 0) {
            game->is_blinking = false;
            game->blink_show = true;
        } else {
            if (game->blink_timer % BLINK_INTERVAL == 0) {
                game->blink_show = !game->blink_show;
            }
        }
    }
}

static void game_erase_entities(Game *game) {
    player_erase(&game->player, game->background_color);
    
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (game->player_bullets[i].active) {
            bullet_erase(&game->player_bullets[i], game->background_color);
        }
    }
    
    if (game->alien_bullet.active) {
        bullet_erase(&game->alien_bullet, game->background_color);
    }
    
    alien_swarm_erase(&game->alien_swarm, game->background_color);
}

static void game_update_positions(Game *game) {
    
    if (game->moving_left) {
        player_move_left(&game->player, game->screen_width);
    }
    
    if (game->moving_right) {
        player_move_right(&game->player, game->screen_width);
    }
    
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (game->player_bullets[i].active) {
            if (!bullet_update_player(&game->player_bullets[i])) {
                game->player_bullets[i].active = false;
            }
        }
    }
    
    if (game->alien_bullet.active) {
        if (!bullet_update_alien(&game->alien_bullet, game->screen_height)) {
            game->alien_bullet.active = false;
        }
    }
}

static void game_check_player_hit(Game *game) {
    if (game->alien_bullet.active) {
        if (check_collision(game->alien_bullet.x, game->alien_bullet.y,
                           game->alien_bullet.width, game->alien_bullet.height,
                           game->player.x, game->player.y,
                           game->player.width, game->player.height)) {
            if (!game->is_blinking) {
                game->lives--;
                game->alien_bullet.active = false;
                
                if (game->lives <= 0) {
                    game->state = GAME_STATE_PLAYER_LOST;
                    return;
                }
                
                game->is_blinking = true;
                game->blink_timer = BLINK_DURATION;
                game->blink_show = false;
            } else {
                game->alien_bullet.active = false;
            }
        }
    }
}

static void game_aliens_shoot(Game *game) {
    int shooter_index;
    if (game->alien_bullet.active == false && 
        alien_swarm_get_shooter(&game->alien_swarm, &shooter_index)) {
        bullet_fire_from_alien(&game->alien_bullet,
                               game->alien_swarm.aliens[shooter_index].x,
                               game->alien_swarm.aliens[shooter_index].y,
                               game->alien_swarm.aliens[shooter_index].width,
                               game->alien_swarm.aliens[shooter_index].height);
    }
}

static void game_check_player_bullet_shield_collision(Game *game) {
    for (int b = 0; b < MAX_PLAYER_BULLETS; b++) {
        if (game->player_bullets[b].active) {
            for (int i = 0; i < SHIELD_COUNT; i++) {
                if (game->shields.shields[i].alive) {
                    if (check_collision(game->player_bullets[b].x, game->player_bullets[b].y,
                                       game->player_bullets[b].width, game->player_bullets[b].height,
                                       game->shields.shields[i].x, game->shields.shields[i].y,
                                       game->shields.shields[i].width, game->shields.shields[i].height)) {
                        shield_erase(&game->shields, i, game->background_color);
                        shield_destroy(&game->shields, i);
                        game->player_bullets[b].active = false;
                        break;
                    }
                }
            }
        }
    }
}

static void game_check_alien_bullet_shield_collision(Game *game) {
    if (game->alien_bullet.active) {
        for (int i = 0; i < SHIELD_COUNT; i++) {
            if (game->shields.shields[i].alive) {
                if (check_collision(game->alien_bullet.x, game->alien_bullet.y,
                                   game->alien_bullet.width, game->alien_bullet.height,
                                   game->shields.shields[i].x, game->shields.shields[i].y,
                                   game->shields.shields[i].width, game->shields.shields[i].height)) {
                    shield_erase(&game->shields, i, game->background_color);
                    shield_destroy(&game->shields, i);
                    game->alien_bullet.active = false;
                    break;
                }
            }
        }
    }
}

static void game_check_player_bullet_alien_collision(Game *game) {
    for (int b = 0; b < MAX_PLAYER_BULLETS; b++) {
        if (game->player_bullets[b].active) {
            for (int i = 0; i < ALIEN_COUNT; i++) {
                if (game->alien_swarm.aliens[i].alive) {
                    if (check_collision(game->player_bullets[b].x, game->player_bullets[b].y,
                                       game->player_bullets[b].width, game->player_bullets[b].height,
                                       game->alien_swarm.aliens[i].x, game->alien_swarm.aliens[i].y,
                                       game->alien_swarm.aliens[i].width, game->alien_swarm.aliens[i].height)) {
                        alien_kill(&game->alien_swarm, i);
                        game->player_bullets[b].active = false;
                        game->score += 100;
                        
                        if (alien_swarm_all_dead(&game->alien_swarm)) {
                            game->state = GAME_STATE_PLAYER_WON;
                            return;
                        }
                        
                        break;
                    }
                }
            }
        }
    }
}

/* --- Função Principal de Atualização do Jogo --- */

void game_update(Game *game) {
    if (game == NULL || game->state != GAME_STATE_PLAYING) {
        return;
    }
    
    game_update_blinking(game);
    game_erase_entities(game);
    game_update_positions(game);
    
    game_check_player_hit(game);
    if (game->state != GAME_STATE_PLAYING) {
        return;
    }
    
    alien_swarm_update(&game->alien_swarm);
    
    if (alien_swarm_reached_player(&game->alien_swarm, game->player.y)) {
        game->state = GAME_STATE_PLAYER_LOST;
        return;
    }
    
    game_aliens_shoot(game);
    
    game_check_player_bullet_shield_collision(game);
    game_check_alien_bullet_shield_collision(game);
    game_check_player_bullet_alien_collision(game);
}

void game_draw(Game *game) {
    if (game == NULL) {
        return;
    }
    
    if (!game->is_blinking || game->blink_show) {
        player_draw(&game->player);
    }
    
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (game->player_bullets[i].active) {
            bullet_draw(&game->player_bullets[i]);
        }
    }
    
    if (game->alien_bullet.active) {
        bullet_draw(&game->alien_bullet);
    }
    
    alien_swarm_draw(&game->alien_swarm);
    shield_array_draw(&game->shields);
    
    graphics_draw_rectangle(LIVES_START_X, LIVES_START_Y, LIVES_CLEAR_WIDTH, LIVES_HEIGHT, game->background_color);
    for (int i = 0; i < game->lives; i++) {
        graphics_draw_rectangle(LIVES_START_X + i * LIVES_SPACING, LIVES_START_Y, LIVES_WIDTH, LIVES_HEIGHT, LIVES_COLOR);
    }
}

void game_erase(Game *game) {
    if (game == NULL) {
        return;
    }
    
    player_erase(&game->player, game->background_color);
    
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (game->player_bullets[i].active) {
            bullet_erase(&game->player_bullets[i], game->background_color);
        }
    }
    
    if (game->alien_bullet.active) {
        bullet_erase(&game->alien_bullet, game->background_color);
    }
    
    alien_swarm_erase(&game->alien_swarm, game->background_color);
    
    for (int i = 0; i < SHIELD_COUNT; i++) {
        if (game->shields.shields[i].alive) {
            shield_erase(&game->shields, i, game->background_color);
        }
    }
}

void game_set_player_movement(Game *game, bool moving_left, bool moving_right) {
    if (game == NULL) {
        return;
    }
    
    game->moving_left = moving_left;
    game->moving_right = moving_right;
}

void game_fire_player_bullet(Game *game) {
    if (game == NULL || game->state != GAME_STATE_PLAYING) {
        return;
    }
    
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (!game->player_bullets[i].active) {
            bullet_fire_from_player(&game->player_bullets[i],
                                   game->player.x, game->player.y,
                                   game->player.width, game->player.height);
            break;
        }
    }
}

GameState game_get_state(const Game *game) {
    if (game == NULL) {
        return GAME_STATE_MENU;
    }
    
    return game->state;
}

int game_get_score(const Game *game) {
    if (game == NULL) {
        return 0;
    }
    
    return game->score;
}

int game_get_lives(const Game *game) {
    if (game == NULL) {
        return 0;
    }
    
    return game->lives;
}

void game_handle_keyboard_input(Game *game, uint8_t scancode, bool *running) {
    if (game == NULL || running == NULL) {
        return;
    }
    
    if (scancode == KBD_ESC_BREAK) {
        *running = false;
        return;
    }
    
    if (game->state == GAME_STATE_MENU) {
        if (scancode == KBD_SPACE_MAKE) {
            game_start(game);
        }
        return;
    }
    
    /* Jogo em progresso */
    if (game->state == GAME_STATE_PLAYING) {
        /* Movimento para a esquerda */
        if (scancode == KBD_A_MAKE) {
            game->moving_left = true;
        } else if (scancode == KBD_A_BREAK) {
            game->moving_left = false;
        }
        
        /* Movimento para a direita */
        if (scancode == KBD_D_MAKE) {
            game->moving_right = true;
        } else if (scancode == KBD_D_BREAK) {
            game->moving_right = false;
        }
        
        /* Disparo */
        if (scancode == KBD_SPACE_MAKE) {
            game_fire_player_bullet(game);
        }
    }
}

