#include "games.h"

#include <malloc.h>

typedef struct chess_c {
    game_c super;
} chess_c;

typedef struct chess_t {
    game_t super;
} chess_t;

int chess_run(game_t *game) {
    chess_t *self = (chess_t*)game;

    return 0;
}

void chess_drop(bobj_t *obj) {
    chess_t *self = (chess_t*)obj;
    game_c_impl()->super.drop((bobj_t*)&self->super);
}

vft_creator(
    chess_c,
    chess_c_impl,
    (chess_c){
        .super = (game_c){
            .super = (bobj_c){
                .name = "chess",
                .drop = chess_drop,
                .size = sizeof(chess_t) - sizeof(chess_c*),
            },
            .run = chess_run,
        },
    }
)

void chess_new(chess_t *self) {
    game_new(&self->super, "chess");
    vft_cast(chess_c, self) = chess_c_impl();
}

game_t *chess(void) {
    chess_t *chess = malloc(sizeof(*chess));
    chess_new(chess);
    return (game_t*)chess;
}
