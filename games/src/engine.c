#include "engine.h"
#include "bobj.h"
#include "term/term.h"

#include <malloc.h>
#include <string.h>

void engine_add(engine_t *self, game_t *game) {
    if(self->games_count == self->games_cap) {
        self->games_cap *= 2;
        self->games = realloc(self->games, self->games_cap);
    }
    self->games[self->games_count] = game;
    self->games_count += 1;
}

void engine_drop(bobj_t *erased_self) {
    engine_t *self = (engine_t*)erased_self;
    while(self->games_count > 0) {
        bobj_drop((bobj_t*)self->games[self->games_count - 1]);
        self->games_count -= 1;
    }
    free(self->games);
}

int engine_run(engine_t *self) {
    
    game_run(self->games[0]);
    int ch = 0;
    while((ch = term_readch()) != 27) {
        /*switch(ch) {
            case KEY_DOWN: menu_driver(game_menu, REQ_DOWN_ITEM); break;
            case KEY_UP: menu_driver(game_menu, REQ_UP_ITEM); break;
            case '\n': {
                size_t selected = item_index(current_item(game_menu));
                clear();
                refresh();
                game_run(self->games[selected], stdscr);
                return 0;
            } break;
        }*/
    }
    
    return 0;
}

vft_creator(
    engine_c,
    engine_c_impl,
    (engine_c){
        .super = (bobj_c){
            .drop = engine_drop,
            .size = sizeof(engine_t) - sizeof(engine_c*),
            .name = "engine",
        }
    }    
)

void engine_new(engine_t *self) {
    vft_cast(engine_c, self) = engine_c_impl();
    self->games = calloc(sizeof(game_t*), 5);
    self->games_count = 0;
    self->games_cap = 5;
}

int game_run(game_t *game) {
    return vft_cast(game_c, game)->run(game);
}

void game_drop(bobj_t *obj) {
    game_t *self = (game_t*)obj;
    free(self->name);
}

void game_new(game_t *game, char *name) {
    vft_cast(game_c, game) = game_c_impl();
    game->name = strdup(name);
}

vft_creator(
    game_c,
    game_c_impl,
    (game_c){
        .super = (bobj_c){
            .drop = game_drop,
            .size = sizeof(game_t) - sizeof(game_c),
            .name = "game",
        },
        .run = (void*)bobj_virtual,
    }
)

