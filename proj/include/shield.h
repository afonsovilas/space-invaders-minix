#ifndef SHIELD_H
#define SHIELD_H

#include <stdint.h>
#include <stdbool.h>

#define SHIELD_COUNT 4

/**
 * Estrutura que representa um escudo.
 */
typedef struct {
    int x;                      /* Posição X */
    int y;                      /* Posição Y */
    int width;                  /* Largura */
    int height;                 /* Altura */
    uint32_t color;             /* Cor para renderização */
    bool alive;                 /* Se ainda existe */
} Shield;

/**
 * Estrutura que gerencia todos os escudos.
 */
typedef struct {
    Shield shields[SHIELD_COUNT];
} ShieldArray;

/**
 * Inicializa os escudos no mapa.
 * 
 * @param shields Pointer para a estrutura ShieldArray
 * @return 0 se bem-sucedido, -1 caso contrário
 */
int shield_array_init(ShieldArray *shields);

/**
 * Marca um escudo como destruído.
 * 
 * @param shields Pointer para a estrutura ShieldArray
 * @param shield_index Índice do escudo
 */
void shield_destroy(ShieldArray *shields, int shield_index);

/**
 * Desenha todos os escudos que ainda existem.
 * 
 * @param shields Pointer para a estrutura ShieldArray
 */
void shield_array_draw(const ShieldArray *shields);

/**
 * Apaga um escudo da tela.
 * 
 * @param shields Pointer para a estrutura ShieldArray
 * @param shield_index Índice do escudo
 * @param background_color Cor de fundo
 */
void shield_erase(const ShieldArray *shields, int shield_index, 
                  uint32_t background_color);

#endif /* SHIELD_H */
