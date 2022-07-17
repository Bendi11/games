#include "bobj.h"
#include "games.h"
#include "term/term.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

typedef struct minesweeper_t minesweeper_t;

typedef struct {
    game_c super;
    size_t (*reveal)(minesweeper_t*, uint16_t, uint16_t, bool);
    void (*reset)(minesweeper_t*);
} minesweeper_v;

typedef struct { uint16_t x; uint16_t y; } pos;

static uint8_t BOMB = 9;
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

typedef struct {
    uint8_t flagged;
    uint8_t unrevealed;
} near_t;

typedef struct minesweeper_t {
    game_t super;
    pos cursor;
    near_t cursor_near;
    tile_t *board;
    gamestate_e state;
    uint16_t cols;
    uint16_t lines;
    uint16_t bombs;
    uint16_t unrevealed;
    time_t start;
} minesweeper_t;

static term_color_t flag_fg = {255, 50, 50};
static term_color_t grey = {100, 100, 100};
static term_color_t darkgrey = {50, 50, 50};
static term_color_t cursor = {255, 255, 0};

static term_color_t neighbor_colors[10] = {
    {0, 0, 0},
    {50, 100, 255},
    {25, 150, 25},
    {255, 0, 0},
    {50, 75, 150},
    {200, 50, 50},
    {0, 150, 150},
    {0, 0, 0},
    {50, 50, 50},
    {255, 0, 0},
};

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
    srand(time(NULL));
    self->state = STATE_RUNNING;
    self->bombs = 0;
    self->start = 0;
    memset(self->board, 0, sizeof(tile_t) * self->lines * self->cols);
    for(uint16_t i = 0; i < self->lines * self->cols; ++i) {
        if(rand() < RAND_MAX / 7) {
            self->board[i].neighbors = BOMB; 
            self->bombs += 1;
        }
    }
    self->unrevealed = self->lines * self->cols;

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
    tile_t *tile = &self->board[idx];
    if(!tile->revealed) {
        term_attr((term_attr_t){.reset_all = true});
    } else {
        term_attr((term_attr_t){.bold = true});
        term_fg(neighbor_colors[tile->neighbors]);
        term_bg(grey);
    }
    if(tile->flagged) {
        term_fg(flag_fg);
    }

    term_cursor(idx % self->cols, idx / self->cols);
    fputc(tile->flagged ? 'F' :
        tile->revealed && tile->neighbors != 0 ?
           (tile->neighbors == BOMB ? 'B' : tile->neighbors + '0') :
           ' ',
        stdout
    );
    term_attr((term_attr_t){.reset_style = true});
}

static size_t minesweeper_reveal(minesweeper_t *self, uint16_t x, uint16_t y, bool autoreveal) {
    if(self->start == 0) { self->start = time(NULL); }
    tile_t *current = &self->board[IDX(x, y)];
    if(current->flagged || (current->revealed && autoreveal)) { return 0; }
    !current->revealed && (self->unrevealed -= 1);
    current->revealed = true;
    minesweeper_rendertile(self, IDX(x, y));
    if(current->neighbors == BOMB) {
        self->state = STATE_LOST;
        return 1;
    } else if(self->unrevealed == self->bombs) {
        self->state = STATE_WIN;
        return 1;
    }

    uint16_t neighbors[MAX_NEIGHBORS + 1];
    minesweeper_neighbors(self, x, y, neighbors);
    uint8_t flagged = 0;
    for(uint8_t i = 0; i < MAX_NEIGHBORS && neighbors[i] != INVALID_IDX; ++i) {
        if(self->board[neighbors[i]].flagged) { flagged += 1; }
    }

    size_t revealed = 1;

    if(flagged == current->neighbors) {
        for(uint8_t i = 0; i < MAX_NEIGHBORS && neighbors[i] != INVALID_IDX; ++i) {
            revealed += minesweeper_reveal(self, neighbors[i] % self->cols, neighbors[i] / self->cols, true);
        }
    }

    return revealed;
}

static near_t minesweeper_near(minesweeper_t *self, pos p) {
    near_t near = {0, 0};
    uint16_t neighbors[MAX_NEIGHBORS + 1];
    minesweeper_neighbors(self, p.x, p.y, neighbors);
    for(uint8_t i = 0; i < MAX_NEIGHBORS && neighbors[i] != INVALID_IDX; ++i) {
        tile_t *tile = &self->board[neighbors[i]];
        if(!tile->revealed) {
            near.unrevealed += 1;
            if(tile->flagged) {
                near.flagged += 1;
            }
        }
    }
    return near;
}

static term_color_t minesweeper_highlight_color(tile_t tile, near_t near) {
    return  (tile.neighbors == near.flagged) ?
        (term_color_t){.r = 25, .g = 100, .b = 75} : 
        (near.unrevealed == tile.neighbors) ?
            (term_color_t){.r = 150, .g = 50, .b = 50} :
            (term_color_t){.r = 100, .g = 100, .b = 50};
 
}

/** \brief Re-render all text around the cursor, highlighted if it is in range of the cursor and unrevealed */
static void minesweeper_highlight(minesweeper_t *self, pos p, bool highlight) {
    near_t near = (highlight) ? minesweeper_near(self, p) : (near_t){0};
    if(highlight) {
        self->cursor_near = near;
    }
    
    uint16_t neighbors[MAX_NEIGHBORS + 1];
    minesweeper_neighbors(self, p.x, p.y, neighbors);
    for(uint8_t i = 0; i < MAX_NEIGHBORS && neighbors[i] != INVALID_IDX; ++i) {
        uint16_t idx = neighbors[i];
        if(highlight &&
            self->board[IDX(p.x, p.y)].revealed &&
            !self->board[idx].revealed &&
            !self->board[idx].flagged
        ) {
            term_bg(minesweeper_highlight_color(self->board[IDX(p.x, p.y)], near));
            term_cursor(idx % self->cols, idx / self->cols);
            fputc(' ', stdout);
        } else {
            minesweeper_rendertile(self, idx);
        }
    }
}

static size_t minesweeper_auto(minesweeper_t *self) {
    size_t revealed = 0;
    uint16_t *to_flag = calloc(self->lines * self->cols, sizeof(*to_flag));
    uint16_t *to_reveal = calloc(self->lines * self->cols, sizeof(*to_flag));
    uint32_t to_reveal_n = 0;
    uint32_t to_flag_n = 0;
    for(uint32_t i = 0; i < self->lines * self->cols; ++i) {
        if(!self->board[i].revealed || self->board[i].neighbors == 0) continue;
        near_t near = minesweeper_near(self, (pos){.x = i % self->cols, .y = i / self->cols});
        uint16_t neighbors[MAX_NEIGHBORS + 1];
        minesweeper_neighbors(self, i % self->cols, i / self->cols, neighbors);
        for(uint8_t j = 0; j < MAX_NEIGHBORS && neighbors[j] != INVALID_IDX; ++j) {
            uint16_t idx = neighbors[j];
            if(
                !self->board[idx].revealed &&
                !self->board[idx].flagged
            ) {
                if(self->board[i].neighbors != near.flagged && near.unrevealed != self->board[i].neighbors) continue;
                term_bg(minesweeper_highlight_color(self->board[i], near));
                term_cursor(idx % self->cols, idx / self->cols);
                fputc(' ', stdout);
                if(self->board[i].neighbors == near.flagged) {
                    to_reveal[to_reveal_n++] = idx;
                } else if(self->board[i].neighbors == near.unrevealed) {
                    to_flag[to_flag_n++] = idx;
                }
            }
        }
    }
    
    if(to_flag_n == 0 && to_reveal_n == 0) return 0;
    term_refresh();
    usleep(1000000);

    revealed += to_flag_n;
    for(uint32_t i = 0; i < to_flag_n; ++i) {
        self->board[to_flag[i]].flagged = true;
        minesweeper_rendertile(self, to_flag[i]);
    }
    for(uint32_t i = 0; i < to_reveal_n; ++i) {
        revealed += minesweeper_reveal(self, to_reveal[i] % self->cols, to_reveal[i] / self->cols, false);
    }

    term_refresh();
    return revealed;
}

static void minesweeper_renderbottom(minesweeper_t * self) {
    char tmbuf[64];
    term_attr((term_attr_t){.reset_all = true});
    time_t duration = time(NULL) - self->start;

    term_cursor(0, self->lines);
    term_clear_line();
    if(self->start == 0) return;
    snprintf(tmbuf, 64, "%zu S", duration);
    term_cursor(self->cols / 2 - strlen(tmbuf), self->lines);
    fputs(tmbuf, stdout);
    term_refresh();
} 

static int minesweeper_run(game_t *game) {
    minesweeper_t *self = (minesweeper_t*)game;
    term_clear();
    term_cursorshow(false);

    for(uint16_t i = 0; i < self->lines * self->cols; ++i) {
        minesweeper_rendertile(self, i);
    }

    int ch = 0;
    bool flash = false;
    do {
        minesweeper_renderbottom(self);
        switch(ch) {
            case TERM_UP:
                minesweeper_highlight(self, self->cursor, false);
                if(self->cursor.y > 0) { self->cursor.y -= 1; }
                minesweeper_highlight(self, self->cursor, true);
            break;
            case TERM_DOWN:
                minesweeper_highlight(self, self->cursor, false);
                if(self->cursor.y < self->lines - 1) { self->cursor.y += 1; } 
                minesweeper_highlight(self, self->cursor, true);
            break;
            case TERM_LEFT:
                minesweeper_highlight(self, self->cursor, false);
                if(self->cursor.x > 0) { self->cursor.x -= 1; }
                minesweeper_highlight(self, self->cursor, true);
            break;
            case TERM_RIGHT:
                minesweeper_highlight(self, self->cursor, false);
                if(self->cursor.x < self->cols) { self->cursor.x += 1; }
                minesweeper_highlight(self, self->cursor, true);
            break;
            case '\n':
            case ' ': minesweeper_reveal(self, self->cursor.x, self->cursor.y, false); break;
            case 'f': {
                tile_t *selected = &self->board[IDX(self->cursor.x, self->cursor.y)];
                if(!selected->revealed) {
                    selected->flagged = !selected->flagged;
                } else if(self->cursor_near.unrevealed == self->board[IDX(self->cursor.x, self->cursor.y)].neighbors) {
                    uint16_t neighbors[MAX_NEIGHBORS + 1];
                    minesweeper_neighbors(self, self->cursor.x, self->cursor.y, neighbors);
                    for(uint8_t i = 0; i < MAX_NEIGHBORS && neighbors[i] != INVALID_IDX; ++i) {
                        !self->board[neighbors[i]].revealed && (self->board[neighbors[i]].flagged = true);
                        minesweeper_rendertile(self, neighbors[i]);
                    }
                }
            } break;
            case 'e': {
                minesweeper_auto(self);
            } break;
            default: {
                minesweeper_renderbottom(self);
                if(flash) {
                    minesweeper_rendertile(self, IDX(self->cursor.x, self->cursor.y));
                    flash = !flash;
                    term_refresh();
                    continue;
                }
                flash = !flash;
            } break;
        }

        if(self->state != STATE_RUNNING) {
            char *text = (self->state == STATE_WIN) ? "You Win!" : "You Lose!";
            size_t len = strlen(text);
            term_cursor(self->cols / 2 - len / 2, self->lines / 2);
            term_attr((term_attr_t){.reset_all = true});
            term_attr((term_attr_t){.bold = true});
            term_fg((term_color_t){255, 255, 0});
            term_bg((term_color_t){0, 0, 0});
            fputs(text, stdout);
            term_refresh();
            term_readch();
            minesweeper_reset(self);
            for(uint16_t i = 0; i < self->lines * self->cols; ++i) {
                minesweeper_rendertile(self, i);
            }
        }

        term_cursor(self->cursor.x, self->cursor.y);
        term_fg(cursor);
        term_bg(self->board[IDX(self->cursor.x, self->cursor.y)].revealed ? grey : darkgrey);
        fputc('X', stdout);
        term_refresh();
    } while((ch = term_readch_timeout(500)) != '\x1b');
    return 0;
}

static void minesweeper_drop(bobj_t *obj) {
    minesweeper_t *self = (minesweeper_t*)obj;
    free(self->board);

    game_c_impl()
        ->super
        .drop((bobj_t*)&self->super); 
}

vft_creator(
    minesweeper_v,
    minesweeper_v_impl,
    (minesweeper_v){
        .super = (game_c){
            .super = (bobj_c){
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
    ms->lines = term_height() - 1;
    ms->cols = term_width();
    ms->cursor = (pos){.x = ms->cols / 2, .y = ms->lines / 2};
    ms->board = calloc(ms->lines * ms->cols, sizeof(tile_t));
    memset(ms->board, 0, sizeof(tile_t) * ms->lines * ms->cols);
    ms->state = STATE_RUNNING;
    minesweeper_reset(ms);
    return (game_t*)ms;
}
