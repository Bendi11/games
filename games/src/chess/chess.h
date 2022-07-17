#pragma once

#include "bobj/iter.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum color_e {
    C_WHITE = 1,
    C_BLACK = 0
} color_e;

typedef enum piecetype_e {
    P_PAWN,
    P_KNIGHT,
    P_BISHOP,
    P_ROOK,
    P_QUEEN,
    P_KING
} piecetype_e;

typedef enum castleside_e {
    CASTLE_QUEENSIDE,
    CASTLE_KINGSIDE
} castleside_e;

typedef enum direction_e {
    D_N,
    D_NE,
    D_E,
    D_SE,
    D_S,
    D_SW,
    D_W,
    D_NW
} direction_e;

typedef struct piece_t {
    uint8_t color : 1;
    uint8_t type : 3;
    bool moved : 1;
    bool exists : 1;
} piece_t;

extern const char *piece_glyphs[2][6];

typedef uint8_t square_t;
static inline square_t square_up(square_t s, uint8_t n) { return s + n * 8; }
static inline square_t square_down(square_t s, uint8_t n) { return s - n * 8; }
static inline square_t square_left(square_t s, uint8_t n) { return s - n; }
static inline square_t square_right(square_t s, uint8_t n) { return s + n; }
__attribute__((flatten))
static inline square_t square_slide(square_t s, uint8_t dir, uint8_t n) {
    switch(dir) {
        case D_N: return square_up(s, n);
        case D_NE: return square_right(square_up(s, n), n);
        case D_E: return square_right(s, n);
        case D_SE: return square_right(square_down(s, n), n);
        case D_S: return square_down(s, n);
        case D_SW: return square_left(square_down(s, n), n);
        case D_W: return square_left(s, n);
        case D_NW: return square_left(square_up(s, n), n);
        default: return 0;
    }
}

static inline bool square_slide_valid(square_t s, uint8_t dir, uint8_t n) {
    uint8_t x = s % 8;
    uint8_t y = s / 8;
    switch(dir) {
        case D_N: return y + n < 8;
        case D_NE: return (x + n < 8) && (y + n < 8);
        case D_E: return x + n < 8;
        case D_SE: return (x + n < 8) && ((uint8_t)(y - n) < 8);
        case D_S: return (uint8_t)(y - n) < 8;
        case D_SW: return ((uint8_t)(x - n) < 8) && ((uint8_t)(y - n) < 8);
        case D_W: return (uint8_t)(x - n) < 8;
        case D_NW: return ((uint8_t)(x - n) < 8) && (y + n < 8);
        default: return 0;
    }
}

#define INVALID_SQUARE ((uint8_t)255)
#define IDX(x, y) (((y) * 8) + (x))
#define VALID(x) ((uint8_t)(x) < 64)

inline static piece_t piece(uint8_t color, uint8_t type) {
    return (piece_t){.color = color, .type = type, .moved = false, .exists = true};
}

typedef struct board_t {
    piece_t pieces[64];
    square_t enpassant;
    uint8_t turn;
} board_t;

typedef struct move_t {
    uint8_t from_rank : 4;
    uint8_t from_file : 4;
    uint8_t from_type : 3;
    square_t to : 6;
    bool promote : 1;
    bool capture : 1;
    bool enpassant : 1;
    bool castle : 1;
    union {
        piece_t promoted;
        uint8_t castle_side;
    };
} move_t;

extern move_t move_default;

typedef struct {
    union {
        uint8_t stage;
        struct {
            uint8_t n : 4;
            uint8_t direction : 4;
        };
    };
} movegen_stage_t;

typedef struct {
    biter_c super;
} chess_move_iter_c;

typedef struct {
    biter_t super;
    board_t *board;
    move_t current;
    movegen_stage_t stage;
    piece_t piece;
    square_t from;
} chess_move_iter_t;

void chess_move_iter_new(chess_move_iter_t *iter, board_t *board, square_t from);
bool chess_move_iter_drive(chess_move_iter_t *moves);

square_t chess_move_valid(board_t *board, move_t move);
bool chess_make_move(board_t *board, move_t move);
