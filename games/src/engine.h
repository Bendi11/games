#pragma once

#include <curses.h>
#include <stddef.h>

#include <bobj.h>

typedef struct game_t game_t;

typedef struct game_i {
    obj_i super;
    /** \brief Function to run for the game */
    int (*run)(game_t*,WINDOW*);
} game_i;

typedef struct game_t {
    obj_t super;
    char *name;
} game_t;

int game_run(game_t *game, WINDOW *win);
void game_new(game_t *game, char *name);

typedef struct gamelist_s {
 
} gamelist_t;

typedef struct engine_s {
    obj_t super;
    game_t **games;
    uint32_t games_count;
    uint32_t games_cap;
} engine_t;

void engine_new(engine_t *self);
void engine_add(engine_t *self, game_t *game);
int engine_run(engine_t *self);
