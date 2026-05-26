#ifndef BULLET_H
#define BULLET_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Estrutura que representa uma bala.
 * Pode ser do jogador ou alienígena.
 */
typedef struct {
    int x;                      /* Posição X */
    int y;                      /* Posição Y */
    int width;                  /* Largura */
    int height;                 /* Altura */
    int speed;                  /* Velocidade */
    uint32_t color;             /* Cor para renderização */
    bool active;                /* Se a bala está ativa */
} Bullet;

/**
 * Inicializa a estrutura de uma bala (jogador).
 * 
 * @param bullet Pointer para a estrutura Bullet
 * @return 0 se bem-sucedido, -1 caso contrário
 */
int bullet_player_init(Bullet *bullet);

/**
 * Inicializa a estrutura de uma bala alienígena.
 * 
 * @param bullet Pointer para a estrutura Bullet
 * @return 0 se bem-sucedido, -1 caso contrário
 */
int bullet_alien_init(Bullet *bullet);

/**
 * Atira uma bala a partir da posição do jogador.
 * 
 * @param bullet Pointer para a estrutura Bullet
 * @param player_x Posição X do jogador
 * @param player_y Posição Y do jogador
 * @param player_width Largura do jogador
 * @param player_height Altura do jogador
 */
void bullet_fire_from_player(Bullet *bullet, int player_x, int player_y, 
                             int player_width, int player_height);

/**
 * Atira uma bala a partir da posição de um alienigena.
 * 
 * @param bullet Pointer para a estrutura Bullet
 * @param alien_x Posição X do alienigena
 * @param alien_y Posição Y do alienigena
 * @param alien_width Largura do alienigena
 * @param alien_height Altura do alienigena
 */
void bullet_fire_from_alien(Bullet *bullet, int alien_x, int alien_y, 
                            int alien_width, int alien_height);

/**
 * Move a bala do jogador (para cima).
 * 
 * @param bullet Pointer para a estrutura Bullet
 * @return true se a bala ainda está ativa, false se saiu da tela
 */
bool bullet_update_player(Bullet *bullet);

/**
 * Move a bala alienígena (para baixo).
 * 
 * @param bullet Pointer para a estrutura Bullet
 * @param screen_height Altura total da tela
 * @return true se a bala ainda está ativa, false se saiu da tela
 */
bool bullet_update_alien(Bullet *bullet, int screen_height);

/**
 * Desenha a bala na tela.
 * 
 * @param bullet Pointer para a estrutura Bullet
 */
void bullet_draw(const Bullet *bullet);

/**
 * Apaga a bala da tela.
 * 
 * @param bullet Pointer para a estrutura Bullet
 * @param background_color Cor de fundo
 */
void bullet_erase(const Bullet *bullet, uint32_t background_color);

/**
 * Desativa a bala.
 * 
 * @param bullet Pointer para a estrutura Bullet
 */
void bullet_deactivate(Bullet *bullet);

#endif /* BULLET_H */
