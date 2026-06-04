#include "alien.h"
#include "graphics.h"
#include "sprites.h"

#include <stddef.h>
#include <stdlib.h>
#include <time.h>

/* Constantes padrão para alienigenas */
#define ALIEN_INITIAL_X_MIN 60
#define ALIEN_INITIAL_X_MAX 220
#define ALIEN_INITIAL_Y_MIN 50
#define ALIEN_INITIAL_Y_MAX 120

#define ALIEN_WIDTH 40
#define ALIEN_HEIGHT 25
#define ALIEN_X_SPACING 20
#define ALIEN_Y_SPACING 20
#define ALIEN_SPEED 2
#define ALIEN_DROP 20
#define ALIEN_COLOR 0x1C
#define ALIEN_MOVE_INTERVAL 10
#define ALIEN_SHOOT_INTERVAL 120

#define ALIEN_BULLET_WIDTH  6
#define ALIEN_BULLET_HEIGHT 14
#define ALIEN_BULLET_SPEED  5
#define ALIEN_BULLET_COLOR  0x3C

const BulletConfig ALIEN_BULLET_CONFIG = {
    ALIEN_BULLET_WIDTH,
    ALIEN_BULLET_HEIGHT,
    ALIEN_BULLET_SPEED,
    ALIEN_BULLET_COLOR,
    BULLET_DIR_DOWN
};

void alien_fire_bullet(Bullet *bullet, const Alien *alien) {
    if (bullet == NULL || alien == NULL) {
        return;
    }

    bullet->active = true;
    bullet->x = alien->x + alien->width  / 2 - bullet->width / 2;
    bullet->y = alien->y + alien->height;
}

/*
  Valid range for random number of aliens.

  ALIEN_COUNT is the maximum possible number of slots:
  ALIEN_ROWS * ALIEN_COLS

  You can adjust ALIEN_MIN_RANDOM_COUNT if you want fewer/more aliens.
*/
#define ALIEN_MIN_RANDOM_COUNT 8

static int random_initialized = 0;

static void alien_random_init(void) {
    if (!random_initialized) {
        srand((unsigned int) time(NULL));
        random_initialized = 1;
    }
}

static int random_between(int min, int max) {
    if (max <= min) {
        return min;
    }

    return min + rand() % (max - min + 1);
}

int alien_swarm_init(AlienSwarm *swarm) {
    if (swarm == NULL) {
        return -1;
    }

    alien_random_init();

    swarm->speed = ALIEN_SPEED;
    swarm->direction = 1;
    swarm->drop = ALIEN_DROP;
    swarm->frame_counter = 0;
    swarm->move_interval = ALIEN_MOVE_INTERVAL;
    swarm->shoot_counter = 0;
    swarm->shoot_interval = ALIEN_SHOOT_INTERVAL;
    swarm->shoot_column = 0;

    /*
      Random number of active rows.
      ALIEN_ROWS is now the maximum, for example 6.
    */
    int active_rows = random_between(3, ALIEN_ROWS);

    /*
      Random formation style:
      0 = full block
      1 = triangle
      2 = inverted triangle
      3 = separated groups
      4 = diamond-ish
    */
    int formation_style = random_between(0, 4);

    /*
      Random starting position for the whole formation.
    */
    int formation_start_x = random_between(60, 180);
    int formation_start_y = random_between(40, 90);

    /*
      First initialize every possible alien slot as inactive.
    */
    for (int row = 0; row < ALIEN_ROWS; row++) {
        for (int col = 0; col < ALIEN_COLS; col++) {
            int i = row * ALIEN_COLS + col;

            swarm->aliens[i].x = formation_start_x + col * (ALIEN_WIDTH + ALIEN_X_SPACING);
            swarm->aliens[i].y = formation_start_y + row * (ALIEN_HEIGHT + ALIEN_Y_SPACING);
            swarm->aliens[i].width = ALIEN_WIDTH;
            swarm->aliens[i].height = ALIEN_HEIGHT;
            swarm->aliens[i].color = ALIEN_COLOR;
            swarm->aliens[i].alive = false;
        }
    }

    /*
      Activate aliens according to the selected formation style.
    */
    for (int row = 0; row < active_rows; row++) {
        for (int col = 0; col < ALIEN_COLS; col++) {
            int i = row * ALIEN_COLS + col;
            bool should_activate = false;

            if (formation_style == 0) {
                /*
                  Full block formation.
                  Example:
                  XXXXXXXX
                  XXXXXXXX
                  XXXXXXXX
                */
                should_activate = true;
            }

            else if (formation_style == 1) {
                /*
                  Triangle formation.
                  Example:
                     XX
                    XXXX
                   XXXXXX
                  XXXXXXXX
                */
                int center_left = (ALIEN_COLS / 2) - 1;
                int center_right = ALIEN_COLS / 2;

                int spread = row;

                if (col >= center_left - spread && col <= center_right + spread) {
                    should_activate = true;
                }
            }

            else if (formation_style == 2) {
                /*
                  Inverted triangle.
                  Example:
                  XXXXXXXX
                   XXXXXX
                    XXXX
                     XX
                */
                int center_left = (ALIEN_COLS / 2) - 1;
                int center_right = ALIEN_COLS / 2;

                int spread = active_rows - row - 1;

                if (col >= center_left - spread && col <= center_right + spread) {
                    should_activate = true;
                }
            }

            else if (formation_style == 3) {
                /*
                  Separated groups.
                  Example:
                  XX  XX  XX
                  XX  XX  XX
                    XX  XX
                  XX  XX  XX
                */
                if (row % 2 == 0) {
                    if (col == 0 || col == 1 ||
                        col == 3 || col == 4 ||
                        col == 6 || col == 7) {
                        should_activate = true;
                    }
                } else {
                    if (col == 1 || col == 2 ||
                        col == 5 || col == 6) {
                        should_activate = true;
                    }
                }
            }

            else if (formation_style == 4) {
                /*
                  Diamond-ish formation.
                  Example:
                    XXXX
                   XXXXXX
                  XXXXXXXX
                   XXXXXX
                    XXXX
                */
                int middle = active_rows / 2;
                int distance_from_middle = row - middle;

                if (distance_from_middle < 0) {
                    distance_from_middle = -distance_from_middle;
                }

                int margin = distance_from_middle;

                if (col >= margin && col < ALIEN_COLS - margin) {
                    should_activate = true;
                }
            }

            /*
              Add a small random variation so the shape is not always identical.
              85% chance to keep each selected alien.
            */
            if (should_activate) {
                int keep = random_between(1, 100);

                if (keep <= 85) {
                    swarm->aliens[i].alive = true;
                }
            }
        }
    }

    /*
      Safety check:
      make sure there is at least one alien.
      If the random variation removed everything, force one alien alive.
    */
    bool has_any_alien = false;

    for (int i = 0; i < ALIEN_COUNT; i++) {
        if (swarm->aliens[i].alive) {
            has_any_alien = true;
            break;
        }
    }

    if (!has_any_alien) {
        int fallback_index = random_between(0, active_rows * ALIEN_COLS - 1);
        swarm->aliens[fallback_index].alive = true;
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

    /*
      Random shooter:
      choose a random column, then use the lowest alive alien in that column.
    */
    bool tried_columns[ALIEN_COLS];

    for (int col = 0; col < ALIEN_COLS; col++) {
        tried_columns[col] = false;
    }

    int columns_tried = 0;

    while (columns_tried < ALIEN_COLS) {
        int col = random_between(0, ALIEN_COLS - 1);

        if (tried_columns[col]) {
            continue;
        }

        tried_columns[col] = true;
        columns_tried++;

        for (int row = ALIEN_ROWS - 1; row >= 0; row--) {
            int i = row * ALIEN_COLS + col;

            if (swarm->aliens[i].alive) {
                *alien_index = i;
                swarm->shoot_counter = 0;
                return true;
            }
        }
    }

    swarm->shoot_counter = 0;
    return false;
}

bool alien_swarm_get_random_shooter_immediate(const AlienSwarm *swarm, int *alien_index) {
    if (swarm == NULL || alien_index == NULL) {
        return false;
    }

    /*
      Choose a random column.
      In that column, pick the lowest alive alien.
      Does NOT use shoot_counter.
      This is for extra simultaneous shots.
    */
    bool tried_columns[ALIEN_COLS];

    for (int col = 0; col < ALIEN_COLS; col++) {
        tried_columns[col] = false;
    }

    int columns_tried = 0;

    while (columns_tried < ALIEN_COLS) {
        int col = random_between(0, ALIEN_COLS - 1);

        if (tried_columns[col]) {
            continue;
        }

        tried_columns[col] = true;
        columns_tried++;

        for (int row = ALIEN_ROWS - 1; row >= 0; row--) {
            int i = row * ALIEN_COLS + col;

            if (swarm->aliens[i].alive) {
                *alien_index = i;
                return true;
            }
        }
    }

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
            graphics_draw_xpm((xpm_map_t) ALIEN_SPRITE,
                              (uint16_t) swarm->aliens[i].x,
                              (uint16_t) swarm->aliens[i].y);
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

void alien_swarm_apply_difficulty(AlienSwarm *swarm, int difficulty) {
    if (swarm == NULL) {
        return;
    }

    if (difficulty < 1) {
        difficulty = 1;
    }

    /*
      Stronger difficulty curve.

      Wave 1:
        normal speed
        normal shooting

      Wave 2:
        noticeably faster movement
        much faster shooting

      Wave 3+:
        gets dangerous quickly
    */

    /*
      Movement interval:
      Lower = aliens move more often.
      Starts at 10.
      Wave 1: 10
      Wave 2: 7
      Wave 3: 5
      Wave 4+: minimum 3
    */
    swarm->move_interval = ALIEN_MOVE_INTERVAL - (difficulty - 1) * 3;

    if (swarm->move_interval < 3) {
        swarm->move_interval = 3;
    }

    /*
      Shoot interval:
      Lower = aliens shoot more often.
      Starts at 120.
      Wave 1: 120
      Wave 2: 80
      Wave 3: 40
      Wave 4+: minimum 18
    */
    swarm->shoot_interval = ALIEN_SHOOT_INTERVAL - (difficulty - 1) * 40;

    if (swarm->shoot_interval < 18) {
        swarm->shoot_interval = 18;
    }

    /*
      Alien horizontal speed:
      Wave 1: 2
      Wave 2: 4
      Wave 3: 6
      Wave 4: 8
      capped at 14
    */
    swarm->speed = ALIEN_SPEED + (difficulty - 1) * 2;

    if (swarm->speed > 14) {
        swarm->speed = 14;
    }

    /*
      Alien drop:
      Wave 1: 20
      Wave 2: 30
      Wave 3: 40
      Wave 4: 50
      capped at 80
    */
    swarm->drop = ALIEN_DROP + (difficulty - 1) * 10;

    if (swarm->drop > 80) {
        swarm->drop = 80;
    }
}

