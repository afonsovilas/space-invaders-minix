#include "background.h"
#include "graphics.h"
#include "sprites/earth_sprite.h"

#include <stdint.h>

#define COL_BG           0x00u
#define COL_STAR         0x0Fu
#define COL_STAR_DIM     0x07u
#define COL_BORDER       0x0Bu  /* cyan frame       */

static const uint16_t STARS[][2] = {
  { 18, 48}, {140, 28}, {290, 66}, {492, 12}, {693, 42}, {898, 22}, {978, 77},
  { 48,148}, {248,128}, {390,162}, {648,138}, {848,168}, {998,152},
  { 98,248}, {348,228}, {598,258}, {798,243}, {948,276},
  { 28,348}, {178,368}, {448,338}, {748,362}, {922,340},
  { 78,448}, {318,468}, {598,452}, {900,476}, {150,490},
  {198,548}, {498,558}, {798,543}, {910,560},
  { 98,648}, {698,658}, {350,640}, {850,635},
  { 55,700}, {400,715}, {700,720}, {950,705},
  {250, 10}, {600,  8}, {800,  5}, {450,740}, {150,750},
  {550,380}, {820,400}, { 75,400}, {200,680}, {700,190},
};
#define STAR_COUNT ((int)(sizeof(STARS) / sizeof(STARS[0])))

void draw_starfield_background(void) {
  graphics_clear(COL_BG);

  for (int i = 0; i < STAR_COUNT; i++) {
    uint32_t col = (i % 3 == 0) ? COL_STAR_DIM : COL_STAR;
    graphics_draw_pixel(STARS[i][0], STARS[i][1], col);
    if (i % 7 == 0)
      graphics_draw_rectangle(STARS[i][0], STARS[i][1], 2, 2, COL_STAR);
  }

  for (int t = 0; t < 2; t++) {
    graphics_draw_hline(0, (uint16_t) t,                              (uint16_t)graphics_get_hres(), COL_BORDER);
    graphics_draw_hline(0, (uint16_t)(graphics_get_vres() - 1u - t), (uint16_t)graphics_get_hres(), COL_BORDER);
  }
}

void draw_game_background(void) {
  draw_starfield_background();
  
  uint16_t screen_w = graphics_get_hres();
  uint16_t screen_h = graphics_get_vres();
  
  /* The Earth sprite is 300x100. We want it centered at the very bottom. */
  uint16_t earth_w = 300;
  uint16_t earth_h = 100;
  
  uint16_t ex = (screen_w / 2) - (earth_w / 2);
  uint16_t ey = screen_h - earth_h;
  
  graphics_draw_xpm((xpm_map_t) EARTH_SPRITE, ex, ey);

  /* Redraw the bottom frame border so the Earth appears slightly behind it */
  for (int t = 0; t < 2; t++) {
    graphics_draw_hline(0, (uint16_t)(screen_h - 1u - t), screen_w, COL_BORDER);
  }
}
