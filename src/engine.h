#pragma once

#include <curses.h>
#include <stddef.h>

#define PUN(ty, ...) (*( ty **) __VA_ARGS__ )

typedef struct obj_t obj_t;
typedef struct obj_i {
    /** \brief Free any memory associated with this object and cleanup all resources */
    void (*drop)(obj_t*);
} obj_i;

typedef struct obj_t {
    obj_i *vft;
} obj_t;

void drop(obj_t *obj);

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
