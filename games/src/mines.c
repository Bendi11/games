#include "games.h"

#include <malloc.h>

typedef struct {
    
} minesweeper_i;

typedef struct minesweeper_t {
    game_t super;
} minesweeper_t;

static int ms_run(game_t *game, WINDOW *win) {
    minesweeper_t *self = (minesweeper_t*)game;
    return 0;
}

static void ms_drop(obj_t *obj) {
    minesweeper_t *self = (minesweeper_t*)obj;
}

static game_i MINESWEEPER_GAME_IMPL = {
    .run = ms_run,
    .super = (obj_i){
        .drop = ms_drop,
    },
};

game_t* minesweeper(void) {
    minesweeper_t *ms = malloc(sizeof(*ms));
    vft_cast(game_i, ms) = &MINESWEEPER_GAME_IMPL;
    ms->super.name = "Minesweeper";
    return (game_t*)ms;
}
