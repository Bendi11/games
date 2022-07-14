#include "bobj.h"
#include "games.h"

#include <ncurses.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

typedef struct minesweeper_t minesweeper_t;

typedef struct {
    game_v super;
    void (*reveal)(minesweeper_t*, uint16_t, uint16_t, bool);
    void (*reset)(minesweeper_t*);
} minesweeper_v;

typedef struct { uint16_t x; uint16_t y; } pos;

static uint8_t BOMB = 255;
#define IDX(x, y) (((y) * self->cols) + (x))

typedef struct {
    uint8_t neighbors;
    bool revealed : 1;
    bool flagged : 1;
} tile_t;

typedef enum {
    STATE_RUNNING,
    STATE_LOST,
    STATE_WIN,
} gamestate_e;

typedef struct minesweeper_t {
    game_t super;
    pos cursor;
    tile_t *board;
    gamestate_e state;
    uint16_t cols;
    uint16_t lines;
    WINDOW *win;
} minesweeper_t;

#define COLOR_GREY 8
#define COLOR_DARKGREY 9
#define COLOR_DARKBLUE 10
#define COLOR_DARKRED 11
#define GREY_PAIR 10
#define DARKGREY_PAIR 11
#define CURSOR_PAIR 12
#define FLAG_PAIR 13

#define COLORS_OFFSET 1

#define MAX_NEIGHBORS 8
#define INVALID_IDX ((uint16_t)-1)

static void minesweeper_neighbors(minesweeper_t *self, uint16_t x, uint16_t y, uint16_t buf[]) {
    uint8_t i = 0;
    if((uint16_t)(x - 1) < self->cols) {
        if((uint16_t)(y - 1) < self->lines) { buf[i++] = IDX(x - 1, y - 1); }
        if(y < self->lines) { buf[i++] = IDX(x - 1, y); }
        if((uint16_t)(y + 1) < self->lines) { buf[i++] = IDX(x - 1, y + 1); }
    }

    if((uint16_t)(y + 1) < self->lines) { buf[i++] = IDX(x, y + 1); }
    if((uint16_t)(y - 1) < self->lines) { buf[i++] = IDX(x, y - 1); }

    if((uint16_t)(x + 1) < self->cols) {
        if((uint16_t)(y - 1) < self->lines) { buf[i++] = IDX(x + 1, y - 1); }
        if(y < self->lines) { buf[i++] = IDX(x + 1, y); }
        if((uint16_t)(y + 1) < self->lines) { buf[i++] = IDX(x + 1, y + 1); }
    }
    buf[i] = INVALID_IDX;
}

static void minesweeper_reset(minesweeper_t *self) {
    self->state = STATE_RUNNING;
    memset(self->board, 0, sizeof(tile_t) * self->lines * self->cols);
    for(uint16_t i = 0; i < self->lines * self->cols; ++i) {
        if(rand() < RAND_MAX / 10) {
            self->board[i].neighbors = BOMB; 
        }
    }

    uint16_t neighbors[MAX_NEIGHBORS + 1];
    for(uint16_t i = 0; i < self->lines * self->cols; ++i) {
        if(self->board[i].neighbors == BOMB) { continue; }
        minesweeper_neighbors(self, i % self->cols, i / self->cols, neighbors);
        
        for(uint8_t j = 0; j < MAX_NEIGHBORS && neighbors[j] != INVALID_IDX; ++j) {
            if(self->board[neighbors[j]].neighbors == BOMB) {
                self->board[i].neighbors += 1;
            }    
        }
    }
}

static void minesweeper_rendertile(minesweeper_t *self, uint16_t idx) {
    attrset(A_NORMAL);
    tile_t *tile = &self->board[idx];
    if(tile->flagged) {
        attrset(COLOR_PAIR(FLAG_PAIR));
    } else if(!tile->revealed) {
        attrset(COLOR_PAIR(GREY_PAIR));
    } else {
        attrset(COLOR_PAIR(COLORS_OFFSET + tile->neighbors) | A_NORMAL);
    }

    mvwaddch(self->win, idx / self->cols, idx % self->cols,
        tile->flagged ? 'F' :
             tile->revealed && tile->neighbors != 0 ?
                (tile->neighbors == BOMB ? 'B' : tile->neighbors + '0') :
                ' '
    );
}


static void minesweeper_reveal(minesweeper_t *self, uint16_t x, uint16_t y, bool autoreveal) {
    tile_t *current = &self->board[IDX(x, y)];
    if(current->flagged || (current->revealed && autoreveal)) { return; }
    current->revealed = true;
    minesweeper_rendertile(self, IDX(x, y));
    if(current->neighbors == BOMB) {
        self->state = STATE_LOST;

        return;
    }

    uint16_t neighbors[MAX_NEIGHBORS + 1];
    minesweeper_neighbors(self, x, y, neighbors);
    uint8_t flagged = 0;
    for(uint8_t i = 0; i < MAX_NEIGHBORS && neighbors[i] != INVALID_IDX; ++i) {
        if(self->board[neighbors[i]].flagged) { flagged += 1; }
    }

    if(flagged == current->neighbors) {
        for(uint8_t i = 0; i < MAX_NEIGHBORS && neighbors[i] != INVALID_IDX; ++i) {
            minesweeper_reveal(self, neighbors[i] % self->cols, neighbors[i] / self->cols, true);
        }
    }
}

static int minesweeper_run(game_t *game, WINDOW *win) {
    minesweeper_t *self = (minesweeper_t*)game;
    self->win = win;
    wclear(win);
    start_color();
    init_color(COLOR_GREY, 500, 500, 500);
    init_color(COLOR_DARKGREY, 250, 250, 250);
    init_color(COLOR_DARKBLUE, 250, 250, 1000);
    init_color(COLOR_DARKRED, 500, 250, 250);
    init_color(COLOR_GREEN, 100, 750, 100);
    init_color(COLOR_BLUE, 250, 250, 750);
    init_color(COLOR_RED, 750, 200, 200);
      
    init_pair(COLORS_OFFSET + 0, COLOR_DARKGREY, COLOR_GREY);
    init_pair(COLORS_OFFSET + 1, COLOR_BLUE, COLOR_GREY);
    init_pair(COLORS_OFFSET + 2, COLOR_GREEN, COLOR_GREY);
    init_pair(COLORS_OFFSET + 3, COLOR_RED, COLOR_GREY);
    init_pair(COLORS_OFFSET + 4, COLOR_DARKBLUE, COLOR_GREY);
    init_pair(COLORS_OFFSET + 5, COLOR_DARKRED, COLOR_GREY);
    init_pair(COLORS_OFFSET + 6, COLOR_CYAN, COLOR_GREY);
    init_pair(COLORS_OFFSET + 7, COLOR_BLACK, COLOR_GREY);
    init_pair(COLORS_OFFSET + 8, COLOR_DARKGREY, COLOR_GREY);

    init_pair(GREY_PAIR, COLOR_GREY, COLOR_DARKGREY);
    init_pair(DARKGREY_PAIR, COLOR_DARKGREY, COLOR_GREY);
    init_pair(CURSOR_PAIR, COLOR_YELLOW, COLOR_GREY);
    init_pair(FLAG_PAIR, COLOR_RED, COLOR_DARKGREY);
    curs_set(FALSE);
    
    for(uint16_t i = 0; i < self->lines * self->cols; ++i) {
        minesweeper_rendertile(self, i);
    }

    int ch = 0;
    timeout(500);
    bool flash = false;
    do {
        switch(ch) {
            case KEY_UP: 
                minesweeper_rendertile(self, IDX(self->cursor.x, self->cursor.y));
                if(self->cursor.y > 0) { self->cursor.y -= 1; }
            break;
            case KEY_DOWN: 
                minesweeper_rendertile(self, IDX(self->cursor.x, self->cursor.y));
                if(self->cursor.y < self->lines) { self->cursor.y += 1; } 
            break;
            case KEY_LEFT:
                minesweeper_rendertile(self, IDX(self->cursor.x, self->cursor.y));
                if(self->cursor.x > 0) { self->cursor.x -= 1; }
            break;
            case KEY_RIGHT:
                minesweeper_rendertile(self, IDX(self->cursor.x, self->cursor.y));
                if(self->cursor.x < self->cols) { self->cursor.x += 1; } break;
            break;
            case '\n':
            case ' ': minesweeper_reveal(self, self->cursor.x, self->cursor.y, false); break;
            case 'f': {
                tile_t *selected = &self->board[IDX(self->cursor.x, self->cursor.y)];
                if(selected->revealed) break;
                selected->flagged = !selected->flagged;
            } break;
            case ERR: {
                if(flash) {
                    minesweeper_rendertile(self, IDX(self->cursor.x, self->cursor.y));
                    flash = !flash;
                    continue;
                }
                flash = !flash;
            } break;
        }

        if(self->state != STATE_RUNNING) {
            char *text = (self->state == STATE_WIN) ? "You Win!" : "You Lose!";
            size_t len = strlen(text);
            mvwprintw(win, self->lines / 2, self->cols / 2 - len / 2, "%s", text);
            timeout(-1);
            getch();
            timeout(500);
            minesweeper_reset(self);
            for(uint16_t i = 0; i < self->lines * self->cols; ++i) {
                minesweeper_rendertile(self, i);
            }
        }

        //for(uint16_t i = 0; i < self->lines * self->cols; ++i) {
        //    minesweeper_rendertile(self, i);
        //}

        wmove(win, self->cursor.y, self->cursor.x);
        attrset(COLOR_PAIR(CURSOR_PAIR) | A_BOLD | A_BLINK);
        waddch(win, 'X');
        refresh();
    } while((ch = getch()) != 27);


    return 0;
}

static void minesweeper_drop(bobj_t *obj) {
    minesweeper_t *self = (minesweeper_t*)obj;
    free(self->board);

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
        },
        .reveal = minesweeper_reveal,
        .reset = minesweeper_reset,
    }
)

game_t* minesweeper(void) {
    minesweeper_t *ms = malloc(sizeof(*ms));
    game_new(&ms->super, "Minesweeper");
    vft_cast(minesweeper_v, ms) = minesweeper_v_impl();
    ms->lines = LINES;
    ms->cols = COLS;
    ms->cursor = (pos){.x = ms->cols / 2, .y = ms->lines / 2};
    ms->board = calloc(sizeof(tile_t), ms->lines * ms->cols);
    memset(ms->board, 0, sizeof(tile_t) * ms->lines * ms->cols);
    ms->state = STATE_RUNNING;
    minesweeper_reset(ms);
    return (game_t*)ms;
}
