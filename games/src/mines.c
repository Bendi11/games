#include "bobj.h"
#include "games.h"

#include <malloc.h>

typedef struct {
    game_v super;
} minesweeper_v;

typedef struct minesweeper_t {
    game_t super;
} minesweeper_t;

static int minesweeper_run(game_t *game, WINDOW *win) {
    minesweeper_t *self = (minesweeper_t*)game;
    return 0;
}

static void minesweeper_drop(bobj_t *obj) {
    minesweeper_t *self = (minesweeper_t*)obj;

    game_v_impl()
        ->super
        .drop((bobj_t*)&self->super); 
}

vft_creator(
    minesweeper_v,
    minesweeper_v_impl,
    (minesweeper_v){
        .super = (game_v){
            .super = (bobj_v){
                .size = sizeof(minesweeper_t) - sizeof(minesweeper_v*),
                .drop = minesweeper_drop,
            },
            .run = minesweeper_run,
        }
    }
)

game_t* minesweeper(void) {
    minesweeper_t *ms = malloc(sizeof(*ms));
    game_new(&ms->super, "Minesweeper");
    vft_cast(minesweeper_v, ms) = minesweeper_v_impl();
    return (game_t*)ms;
}
