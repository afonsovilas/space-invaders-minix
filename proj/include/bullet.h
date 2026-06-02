#ifndef BULLET_H
#define BULLET_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Direction a bullet travels on screen.
 */
typedef enum {
    BULLET_DIR_UP   = -1,   /**< Moves towards the top of the screen (player bullet). */
    BULLET_DIR_DOWN =  1    /**< Moves towards the bottom of the screen (alien bullet). */
} BulletDirection;

/**
 * Configuration used to initialise a bullet.
 * Each shooter type owns its own instance of this struct.
 */
typedef struct {
    int width;               /**< Bullet width in pixels. */
    int height;              /**< Bullet height in pixels. */
    int speed;               /**< Pixels moved per update tick. */
    uint32_t color;          /**< Render colour. */
    BulletDirection direction; /**< Travel direction. */
} BulletConfig;

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
    BulletDirection direction;  /* Direção de movimento */
} Bullet;

/**
 * Initialises a bullet from a configuration struct.
 *
 * @param bullet  Pointer to the Bullet to initialise.
 * @param config  Configuration describing the bullet's properties and direction.
 * @return 0 on success, -1 if either pointer is NULL.
 */
int bullet_init(Bullet *bullet, const BulletConfig *config);

/**
 * Advances a bullet by one tick in its travel direction.
 * Deactivates the bullet if it leaves the screen boundaries.
 *
 * @param bullet        Pointer to the Bullet to update.
 * @param screen_height Total screen height in pixels (used for the bottom boundary).
 * @return true if the bullet is still active after the update, false otherwise.
 */
bool bullet_update(Bullet *bullet, int screen_height);

/**
 * Inicializa a estrutura de uma bala (jogador).
 * 
 * @param bullet Pointer para a estrutura Bullet
 * @return 0 se bem-sucedido, -1 caso contrário
 */
int bullet_player_init(Bullet *bullet);

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
 * Move a bala do jogador (para cima).
 * 
 * @param bullet Pointer para a estrutura Bullet
 * @return true se a bala ainda está ativa, false se saiu da tela
 */
bool bullet_update_player(Bullet *bullet);

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
