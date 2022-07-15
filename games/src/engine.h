#pragma once

#include <stddef.h>

#include <bobj.h>

#include "term/term.h"

typedef struct game_t game_t;

typedef struct game_v {
    bobj_v super;
    /** \brief Function to run for the game */
    int (*run)(game_t*);
} game_v;

extern game_v* (*game_v_impl)(void);

typedef struct game_t {
    bobj_t super;
    char *name;
} game_t;


int game_run(game_t *game);
void game_new(game_t *game, char *name);

typedef struct gamelist_s {
 
} gamelist_t;

typedef struct {
    bobj_v super;
} engine_v;

typedef struct engine_s {
    bobj_t super;
    game_t **games;
    uint32_t games_count;
    uint32_t games_cap;
} engine_t;

void engine_new(engine_t *self);
void engine_add(engine_t *self, game_t *game);
int engine_run(engine_t *self);
