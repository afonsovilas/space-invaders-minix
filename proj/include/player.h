#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

/**
 * Estrutura que representa o jogador.
 * Contém posição, dimensões, velocidade e estado de movimento.
 */
typedef struct {
    int x;                      /* Posição X do jogador */
    int y;                      /* Posição Y do jogador */
    int width;                  /* Largura do jogador */
    int height;                 /* Altura do jogador */
    int speed;                  /* Velocidade de movimento */
    uint32_t color;             /* Cor para renderização */
    
    int initial_x;              /* Posição inicial X (para reset) */
    int initial_y;              /* Posição inicial Y (para reset) */
} Player;

/**
 * Inicializa a estrutura do jogador com valores padrão.
 * 
 * @param player Pointer para a estrutura Player
 * @return 0 se bem-sucedido, -1 caso contrário
 */
int player_init(Player *player);

/**
 * Move o jogador para a esquerda, respeitando os limites da tela.
 * 
 * @param player Pointer para a estrutura Player
 * @param screen_width Largura total da tela
 */
void player_move_left(Player *player, int screen_width);

/**
 * Move o jogador para a direita, respeitando os limites da tela.
 * 
 * @param player Pointer para a estrutura Player
 * @param screen_width Largura total da tela
 */
void player_move_right(Player *player, int screen_width);

/**
 * Desenha o jogador na tela.
 * 
 * @param player Pointer para a estrutura Player
 */
void player_draw(const Player *player);

/**
 * Apaga o jogador da tela (desenha com cor de fundo).
 * 
 * @param player Pointer para a estrutura Player
 * @param background_color Cor de fundo
 */
void player_erase(const Player *player, uint32_t background_color);

/**
 * Reposiciona o jogador na posição inicial.
 * Utilizado quando o jogador é atingido por uma bala alienígena.
 * 
 * @param player Pointer para a estrutura Player
 */
void player_reset_position(Player *player);

#endif /* PLAYER_H */
