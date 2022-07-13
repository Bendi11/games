#include "games.h"

typedef struct minesweeper_t {
    game_i super;
} minesweeper_t;

static int ms_run(game_i *game, WINDOW *win) {
    minesweeper_t *self = (minesweeper_t*)game;
    return 0;
}

static void ms_drop(obj_i *obj) {
    minesweeper_t *self = (minesweeper_t*)obj;
}

game_i* minesweeper(void) {
    minesweeper_t *ms = malloc(sizeof(*ms));
    ms->super = (game_i) {
        .name = "Minesweeper",
        .run = ms_run,
        .super = (obj_i){
            .drop = ms_drop,
        }
    };
    return (game_i*)ms;
}
