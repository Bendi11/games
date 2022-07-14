#include "engine.h"
#include "bobj.h"

#include <curses.h>
#include <malloc.h>
#include <menu.h>
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
    ITEM **games = calloc(sizeof(ITEM*), self->games_count);
    MENU *game_menu = NULL;
    for(size_t i = 0; i < self->games_count; ++i) {
        games[i] = new_item(self->games[i]->name, "");
    }
    game_menu = new_menu(games);
    post_menu(game_menu);
    refresh();
    int ch = 0;
    while((ch = getch()) != 27) {
        switch(ch) {
            case KEY_DOWN: menu_driver(game_menu, REQ_DOWN_ITEM); break;
            case KEY_UP: menu_driver(game_menu, REQ_UP_ITEM); break;
            case '\n': {
                size_t selected = item_index(current_item(game_menu));
                clear();
                refresh();
                game_run(self->games[selected], stdscr);
                return 0;
            } break;
        }
    }

    for(size_t i = 0; i < self->games_count; ++i) {
        free_item(games[i]);
    }
    free_menu(game_menu);
    
    return 0;
}

vft_creator(
    engine_v,
    engine_v_impl,
    (engine_v){
        .super = (bobj_v){
            .drop = engine_drop,
            .size = sizeof(engine_t) - sizeof(engine_v*),
            .name = "engine",
        }
    }    
)

void engine_new(engine_t *self) {
    vft_cast(engine_v, self) = engine_v_impl();
    self->games = calloc(sizeof(game_t*), 5);
    self->games_count = 0;
    self->games_cap = 5;
}

int game_run(game_t *game, WINDOW *win) {
    return vft_cast(game_v, game)->run(game, win);
}

void game_drop(bobj_t *obj) {
    game_t *self = (game_t*)obj;
    free(self->name);
}

void game_new(game_t *game, char *name) {
    vft_cast(game_v, game) = game_v_impl();
    game->name = strdup(name);
}

vft_creator(
    game_v,
    game_v_impl,
    (game_v){
        .super = (bobj_v){
            .drop = game_drop,
            .size = sizeof(game_t) - sizeof(game_v),
            .name = "game",
        },
        .run = (void*)bobj_virtual,
    }
)

