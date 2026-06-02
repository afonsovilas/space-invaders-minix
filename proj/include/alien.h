#ifndef ALIEN_H
#define ALIEN_H

#include <stdint.h>
#include <stdbool.h>
#include "bullet.h"

#define ALIEN_ROWS 6
#define ALIEN_COLS 8
#define ALIEN_COUNT (ALIEN_ROWS * ALIEN_COLS)

/**
 * Estrutura que representa um alienigena.
 */
typedef struct {
    int x;                      /* Posição X */
    int y;                      /* Posição Y */
    int width;                  /* Largura */
    int height;                 /* Altura */
    uint32_t color;             /* Cor para renderização */
    bool alive;                 /* Se está vivo */
} Alien;

/**
 * Estrutura que gerencia todos os alienigenas.
 */
typedef struct {
    Alien aliens[ALIEN_COUNT];
    int speed;                  /* Velocidade de movimento */
    int direction;              /* Direção (1 = direita, -1 = esquerda) */
    int drop;                   /* Quantidade de pixels para descer quando muda direção */
    int frame_counter;          /* Contador para controlar movimento gradual */
    int move_interval;          /* Intervalo de frames entre movimentos */
    int shoot_counter;          /* Contador para disparo alienígena */
    int shoot_interval;         /* Intervalo entre disparos */
    int shoot_column;           /* Coluna atual para disparo rotativo */
} AlienSwarm;

/**
 * Inicializa a formação de alienigenas.
 * 
 * @param swarm Pointer para a estrutura AlienSwarm
 * @return 0 se bem-sucedido, -1 caso contrário
 */
int alien_swarm_init(AlienSwarm *swarm);

/**
 * Atualiza a posição de todos os alienigenas.
 * 
 * @param swarm Pointer para a estrutura AlienSwarm
 */
void alien_swarm_update(AlienSwarm *swarm);

/**
 * Verifica se todos os alienigenas foram eliminados.
 * 
 * @param swarm Pointer para a estrutura AlienSwarm
 * @return true se todos estão mortos, false caso contrário
 */
bool alien_swarm_all_dead(const AlienSwarm *swarm);

/**
 * Verifica se algum alienigena atingiu a área do jogador.
 * 
 * @param swarm Pointer para a estrutura AlienSwarm
 * @param player_y Posição Y do jogador
 * @return true se algum alienigena desceu até à altura do jogador
 */
bool alien_swarm_reached_player(const AlienSwarm *swarm, int player_y);

/**
 * Determina qual alienigena deve disparar (o mais baixo da coluna rotativa).
 * 
 * @param swarm Pointer para a estrutura AlienSwarm
 * @param alien_index Pointer para armazenar o índice do alienigena que vai disparar
 * @return true se encontrou um alienigena vivo para disparar, false caso contrário
 */
bool alien_swarm_get_shooter(AlienSwarm *swarm, int *alien_index);

/**
 * Marca um alienigena como morto.
 * 
 * @param swarm Pointer para a estrutura AlienSwarm
 * @param alien_index Índice do alienigena
 */
void alien_kill(AlienSwarm *swarm, int alien_index);

/**
 * Desenha todos os alienigenas vivos.
 * 
 * @param swarm Pointer para a estrutura AlienSwarm
 */
void alien_swarm_draw(const AlienSwarm *swarm);

/**
 * Apaga todos os alienigenas da tela.
 * 
 * @param swarm Pointer para a estrutura AlienSwarm
 * @param background_color Cor de fundo
 */
void alien_swarm_erase(const AlienSwarm *swarm, uint32_t background_color);

/**
 * Bullet configuration for the alien shooter.
 * Pass to bullet_init() when creating an alien bullet.
 */
extern const BulletConfig ALIEN_BULLET_CONFIG;

/**
 * Fires a bullet from the given alien.
 * Positions the bullet at the bottom-centre of the alien and marks it active.
 *
 * @param bullet  Pointer to the Bullet to fire.
 * @param alien   The alien that is shooting.
 */
void alien_fire_bullet(Bullet *bullet, const Alien *alien);

#endif /* ALIEN_H */
