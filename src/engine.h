#pragma once

#include <curses.h>
#include <stddef.h>

typedef struct obj_i {
    /** \brief Free any memory associated with this object and cleanup all resources */
    void (*drop)(struct obj_i*);
} obj_i;


void drop(obj_i *obj);

typedef struct game_i {
    obj_i super;
    /** \brief Function to run for the game */
    int (*run)(struct game_i*,WINDOW*);
    char *name;
} game_i;

typedef struct gamelist_s {
 
} gamelist_t;

typedef struct engine_s {
    obj_i super;
    game_i **games;
    uint32_t games_count;
    uint32_t games_cap;
} engine_t;

engine_t engine_new(void);
void engine_add(engine_t *self, game_i *game);
int engine_run(engine_t *self);
