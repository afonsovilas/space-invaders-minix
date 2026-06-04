#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <stdint.h>
#include "bullet.h"
#include "scoreboard.h"

/** Maximum number of lives the player starts with. */
#define MAX_LIVES 3

/**
 * Estrutura que representa o jogador.
 * Contém posição, dimensões, velocidade, estado de movimento,
 * estatísticas (score, lives, name) e estado visual de blink ao ser atingido.
 */
typedef struct {
    int x;                          /* Posição X do jogador */
    int y;                          /* Posição Y do jogador */
    int width;                      /* Largura do jogador */
    int height;                     /* Altura do jogador */
    int speed;                      /* Velocidade de movimento */
    uint32_t color;                 /* Cor para renderização */

    int initial_x;                  /* Posição inicial X (para reset) */
    int initial_y;                  /* Posição inicial Y (para reset) */

    /* Player stats */
    int score;                      /* Pontuação acumulada */
    int lives;                      /* Vidas restantes */
    char name[SCOREBOARD_NAME_LEN]; /* Nome introduzido no menu */

    /* Input state */
    bool moving_left;               /* Tecla de movimento esquerda pressionada */
    bool moving_right;              /* Tecla de movimento direita pressionada */

    /* Hit-flash visual state */
    bool is_blinking;               /* Verdadeiro enquanto o jogador pisca após ser atingido */
    int  blink_timer;               /* Ticks restantes de animação de blink */
    bool blink_show;                /* Verdadeiro quando o sprite deve ser desenhado durante o blink */
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

/**
 * Redefine o estado de movimento e de blink do jogador.
 * Chamado no início de cada nova onda (wave).
 *
 * @param player Pointer para a estrutura Player
 */
void player_reset_state(Player *player);

/**
 * Atualiza a animação de blink do jogador (deve ser chamado uma vez por tick).
 * Decrementa o timer e alterna a visibilidade do sprite.
 *
 * @param player Pointer para a estrutura Player
 */
void player_update_blinking(Player *player);

/**
 * Retorna verdadeiro se o sprite do jogador deve ser desenhado neste tick.
 * Falso durante o intervalo "escondido" da animação de blink.
 *
 * @param player Pointer para a estrutura Player
 * @return true se visível, false se oculto pelo blink
 */
bool player_is_visible(const Player *player);

/**
 * Processa um impacto no jogador: decrementa uma vida e inicia a animação
 * de blink. Deve ser chamado apenas quando !player->is_blinking.
 * O chamador é responsável por verificar player->lives <= 0.
 *
 * @param player Pointer para a estrutura Player
 */
void player_on_hit(Player *player);

/**
 * Bullet configuration for the player shooter.
 * Pass to bullet_init() when creating a player bullet.
 */
extern const BulletConfig PLAYER_BULLET_CONFIG;

/**
 * Fires a bullet from the given player.
 * Positions the bullet at the top-centre of the player and marks it active.
 *
 * @param bullet  Pointer to the Bullet to fire.
 * @param player  The player that is shooting.
 */
void player_fire_bullet(Bullet *bullet, const Player *player);

#endif /* PLAYER_H */
