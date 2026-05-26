#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdbool.h>
#include "player.h"
#include "bullet.h"
#include "alien.h"
#include "shield.h"

/**
 * Estados do jogo.
 */
typedef enum {
    GAME_STATE_MENU,            /* Menu inicial */
    GAME_STATE_PLAYING,         /* Jogo em progresso */
    GAME_STATE_PLAYER_WON,      /* Jogador venceu */
    GAME_STATE_PLAYER_LOST      /* Jogador perdeu */
} GameState;

/**
 * Estrutura principal do jogo.
 * Contém todos os objetos e estado do jogo.
 */
typedef struct {
    GameState state;
    
    Player player;
    Bullet player_bullet;
    Bullet alien_bullet;
    
    AlienSwarm alien_swarm;
    ShieldArray shields;
    
    int score;
    int lives;
    
    /* Cores */
    uint32_t background_color;
    
    /* Dimensões da tela */
    int screen_width;
    int screen_height;
    
    /* Flags de input */
    bool moving_left;
    bool moving_right;
} Game;

/**
 * Inicializa o estado do jogo.
 * 
 * @param game Pointer para a estrutura Game
 * @param screen_width Largura da tela
 * @param screen_height Altura da tela
 * @return 0 se bem-sucedido, -1 caso contrário
 */
int game_init(Game *game, int screen_width, int screen_height);

/**
 * Inicia um novo jogo (transição do menu para playing).
 * 
 * @param game Pointer para a estrutura Game
 */
void game_start(Game *game);

/**
 * Atualiza toda a lógica do jogo a cada frame.
 * 
 * @param game Pointer para a estrutura Game
 */
void game_update(Game *game);

/**
 * Desenha todos os objetos do jogo.
 * 
 * @param game Pointer para a estrutura Game
 */
void game_draw(Game *game);

/**
 * Apaga todos os objetos do jogo.
 * 
 * @param game Pointer para a estrutura Game
 */
void game_erase(Game *game);

/**
 * Define a direção do movimento do jogador.
 * 
 * @param game Pointer para a estrutura Game
 * @param moving_left true para mover à esquerda
 * @param moving_right true para mover à direita
 */
void game_set_player_movement(Game *game, bool moving_left, bool moving_right);

/**
 * Atira uma bala do jogador (se não houver uma ativa).
 * 
 * @param game Pointer para a estrutura Game
 */
void game_fire_player_bullet(Game *game);

/**
 * Retorna o estado atual do jogo.
 * 
 * @param game Pointer para a estrutura Game
 * @return Estado atual do jogo
 */
GameState game_get_state(const Game *game);

/**
 * Obtém a pontuação atual.
 * 
 * @param game Pointer para a estrutura Game
 * @return Pontuação
 */
int game_get_score(const Game *game);

/**
 * Obtém o número de vidas restantes.
 * 
 * @param game Pointer para a estrutura Game
 * @return Vidas restantes
 */
int game_get_lives(const Game *game);

/**
 * Processa input do teclado (scancode).
 * Trata movimentos do jogador, disparos e menu.
 * 
 * @param game Pointer para a estrutura Game
 * @param scancode Código da tecla pressionada
 * @param running Pointer para flag de saída (alterada se ESC for pressionado)
 */
void game_handle_keyboard_input(Game *game, uint8_t scancode, bool *running);

#endif /* GAME_H */
