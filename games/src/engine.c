#include "engine.h"

#include <curses.h>
#include <malloc.h>
#include <menu.h>


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

static bobj_v ENGINE_OBJ_IMPL = {
    .drop = engine_drop,
};

void engine_new(engine_t *self) {
    vft_cast(bobj_v, self) = &ENGINE_OBJ_IMPL;
    self->games = calloc(sizeof(game_t*), 5);
    self->games_count = 0;
    self->games_cap = 5;
}

int game_run(game_t *game, WINDOW *win) {
    return vft_cast(game_v, game)->run(game, win);
}

void game_new(game_t *game, char *name) {

}
