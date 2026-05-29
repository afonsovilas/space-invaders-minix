#include "game.h"
#include "graphics.h"
#include "keyboard.h"
#include <stddef.h>

/* Constantes de colisão */
#define BACKGROUND_COLOR 0x00
#define LIVES_COLOR 0x0C       /* Vermelho brilhante */
#define BLINK_DURATION 45      /* 0.75 segundos a 60 FPS */
#define BLINK_INTERVAL 6       /* Alterna visibilidade a cada 6 frames */

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
    game->lives = 3;
    
    game->moving_left = false;
    game->moving_right = false;
    
    game->is_blinking = false;
    game->blink_timer = 0;
    game->blink_show = true;
    
    /* Inicializa módulos */
    if (player_init(&game->player) != 0) {
        return -1;
    }
    
    for (int i = 0; i < 3; i++) {
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

void game_update(Game *game) {
    if (game == NULL || game->state != GAME_STATE_PLAYING) {
        return;
    }
    
    /* Atualiza estado de piscar (blinking) */
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
    
    /* Apaga objetos antigos */
    player_erase(&game->player, game->background_color);
    
    for (int i = 0; i < 3; i++) {
        if (game->player_bullets[i].active) {
            bullet_erase(&game->player_bullets[i], game->background_color);
        }
    }
    
    if (game->alien_bullet.active) {
        bullet_erase(&game->alien_bullet, game->background_color);
    }
    
    alien_swarm_erase(&game->alien_swarm, game->background_color);
    
    /* Atualiza movimento do jogador */
    if (game->moving_left) {
        player_move_left(&game->player, game->screen_width);
    }
    
    if (game->moving_right) {
        player_move_right(&game->player, game->screen_width);
    }
    
    /* Atualiza balas */
    for (int i = 0; i < 3; i++) {
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
    
    /* Verifica colisão bala alienígena - jogador */
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
    
    /* Atualiza movimento dos alienigenas */
    alien_swarm_update(&game->alien_swarm);
    
    if (alien_swarm_reached_player(&game->alien_swarm, game->player.y)) {
        game->state = GAME_STATE_PLAYER_LOST;
        return;
    }
    
    int shooter_index;
    if (game->alien_bullet.active == false && 
        alien_swarm_get_shooter(&game->alien_swarm, &shooter_index)) {
        bullet_fire_from_alien(&game->alien_bullet,
                              game->alien_swarm.aliens[shooter_index].x,
                              game->alien_swarm.aliens[shooter_index].y,
                              game->alien_swarm.aliens[shooter_index].width,
                              game->alien_swarm.aliens[shooter_index].height);
    }
    
    /* Colisão bala do jogador - escudo */
    for (int b = 0; b < 3; b++) {
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
    
    /* Colisão bala alienígena - escudo */
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
    
    /* Colisão bala do jogador - alienigena */
    for (int b = 0; b < 3; b++) {
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

void game_draw(Game *game) {
    if (game == NULL) {
        return;
    }
    
    if (!game->is_blinking || game->blink_show) {
        player_draw(&game->player);
    }
    
    for (int i = 0; i < 3; i++) {
        if (game->player_bullets[i].active) {
            bullet_draw(&game->player_bullets[i]);
        }
    }
    
    if (game->alien_bullet.active) {
        bullet_draw(&game->alien_bullet);
    }
    
    alien_swarm_draw(&game->alien_swarm);
    shield_array_draw(&game->shields);
    
    int start_x = 20;
    graphics_draw_rectangle(start_x, 20, 80, 10, game->background_color);
    for (int i = 0; i < game->lives; i++) {
        graphics_draw_rectangle(start_x + i * 30, 20, 20, 10, LIVES_COLOR);
    }
}

void game_erase(Game *game) {
    if (game == NULL) {
        return;
    }
    
    player_erase(&game->player, game->background_color);
    
    for (int i = 0; i < 3; i++) {
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
    
    for (int i = 0; i < 3; i++) {
        if (!game->player_bullets[i].active) {
            bullet_fire_from_player(&game->player_bullets[i],
                                   game->player.x, game->player.y,
                                   game->player.width, game->player.height);
            break;
        }    }
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
    
    /* ESC para sair */
    if (scancode == KBD_ESC_BREAK) {
        *running = false;
        return;
    }
    
    /* Menu: SPACE para começar o jogo */
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

