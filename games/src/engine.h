#pragma once

#include <stddef.h>

#include <bobj.h>

#include "term/term.h"

typedef struct game_t game_t;

typedef struct game_c {
    bobj_c super;
    /** \brief Function to run for the game */
    int (*run)(game_t*);
} game_c;

extern game_c* (*game_c_impl)(void);

typedef struct game_t {
    bobj_t super;
    char *name;
} game_t;

int game_run(game_t *game);
void game_new(game_t *game, char *name);

typedef struct gamelist_s {
 
} gamelist_t;

typedef struct {
    bobj_c super;
} engine_c;

typedef struct engine_t {
    bobj_t super;
    bbuf_list_t games;
} engine_t;

void engine_new(engine_t *self);
void engine_add(engine_t *self, game_t *game);
int engine_run(engine_t *self);
