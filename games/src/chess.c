#include "bobj/iter.h"
#include "games.h"

#include <malloc.h>
#include <memory.h>

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

inline static piece_t piece(uint8_t color, uint8_t type) {
    return (piece_t){.color = color, .type = type, .moved = false, .exists = true};
}

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

typedef struct board_t {
    piece_t pieces[64];
    square_t enpassant;
    uint8_t turn;
} board_t;

typedef struct move_t {
    square_t from : 6;
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
} chess_move_iter_t;

/** \return true If moves->current was set to a new move or false if there are no more moves to generate */
__attribute__((hot))
bool chess_move_iter_drive(chess_move_iter_t *moves) {
    moves->current = (move_t){
        .capture = false,
        .promote = false,
        .enpassant = false,
        .castle = false,
        .from = moves->current.from,
    };
    square_t from = moves->current.from;
    switch(moves->piece.type) {
        case P_PAWN: {
            square_t to;
            bool up_valid = (uint8_t)(moves->piece.color ? (from / 8 + 1) : (from / 8 - 1)) < 8;
            square_t up = moves->piece.color ? square_up(from, 1) : square_down(from, 1);
            switch(moves->stage.stage) {
                case 0: {
                    moves->stage.stage += 1;
                    if(!moves->piece.moved) {
                        to = moves->piece.color ? square_up(from, 2) : square_down(from, 2);
                        if(!moves->board->pieces[to].exists && !moves->board->pieces[up].exists) {
                            moves->current.to = to;
                            return true;
                        }
                    }
                }
                case 1: {
                    moves->stage.stage += 1;
                    if(up_valid) {
                        if(!moves->board->pieces[up].exists) {
                            moves->current.to = up;
                            return true;
                        }
                    }
                }
                case 2: {
                    moves->stage.stage += 1;
                    to = square_left(from, 1);
                    if(up_valid && ((uint8_t)(from % 8 - 1) < 8) && moves->board->pieces[to].exists && moves->board->pieces[to].color != moves->piece.color) {
                        moves->current.to = to;
                        moves->current.capture = true;
                        return true;
                    }
                }
                case 3: {
                    moves->stage.stage += 1;
                    to = square_right(from, 1);
                    if(up_valid && ((uint8_t)(from % 8 + 1) < 8) && moves->board->pieces[to].exists && moves->board->pieces[to].color != moves->piece.color) {
                        moves->current.to = to;
                        moves->current.capture = true;
                        return true;
                    }
                }
            }
        } break;
        case P_KNIGHT: {
            square_t to;
#define ATTACK(n, dx, dy) \
                case n: { \
                    moves->stage.stage += 1;   \
                    uint8_t x = (from % 8) dx; \
                    uint8_t y = (from / 8) dy; \
                    to = IDX(x, y);            \
                    if(x < 8 && y < 8 && (!moves->board->pieces[to].exists || moves->board->pieces[to].color == moves->piece.color)) {     \
                        moves->current.to = to;                                                                                            \
                        moves->current.capture = true;                                                                                     \
                        return true;                                                                                                       \
                    }                                                                                                                      \
                }
            switch(moves->stage.stage) {
                ATTACK(0, + 2, + 1)
                ATTACK(1, + 2, - 1)
                ATTACK(2, + 1, + 2)
                ATTACK(3, - 1, + 2)
                ATTACK(4, - 2, + 1)
                ATTACK(5, - 2, - 1)
                ATTACK(6, + 1, - 2)
                ATTACK(7, - 1, - 2)
            }
        } break;
        case P_BISHOP: 
        case P_ROOK: 
        case P_QUEEN: {
            uint8_t step = moves->piece.type == P_QUEEN ? 1 : 2;
            uint8_t direction = moves->stage.direction;
            uint8_t n = moves->stage.n;
            while(direction <= D_NW) {
                if(square_slide_valid(from, direction, n)) {
                    square_t to = square_slide(from, direction, n);
                    if(moves->board->pieces[to].exists) {
                        if(moves->board->pieces[to].color != moves->piece.color) {
                            moves->current.to = to;
                            moves->current.capture = true;
                            moves->stage.direction = direction;
                            moves->stage.n = n;
                            return true;
                        } else {
                            direction += step;
                            n = 0;
                        }
                    } else {
                        moves->current.to = to;
                        moves->stage.direction = direction;
                        moves->stage.n = n + 1;
                        return true;
                    }
                } else {
                    direction += step;
                    n = 0;
                }
           }
        }
        case P_KING: {
            square_t to;
            switch(moves->stage.stage) {
                ATTACK(0, + 1, + 0)
                ATTACK(1, + 1, + 1)
                ATTACK(2, + 1, - 1)
                ATTACK(3, + 0, + 1)
                ATTACK(4, + 0, - 1)
                ATTACK(5, - 1, + 0)
                ATTACK(6, - 1, + 1)
                ATTACK(7, - 1, - 1)
            }
        }
    }
#undef ATTACK
    return false;
}

void board_new(board_t *board) {
    board->enpassant = INVALID_SQUARE;
    board->turn = C_WHITE;
    
    memset(board->pieces, 0, 64);
    board->pieces[IDX(0, 0)] = board->pieces[IDX(7, 0)] = piece(C_WHITE, P_ROOK);
    board->pieces[IDX(1, 0)] = board->pieces[IDX(6, 0)] = piece(C_WHITE, P_BISHOP);
    board->pieces[IDX(2, 0)] = board->pieces[IDX(5, 0)] = piece(C_WHITE, P_KNIGHT);
    board->pieces[IDX(3, 0)] = piece(C_WHITE, P_QUEEN);
    board->pieces[IDX(4, 0)] = piece(C_WHITE, P_KING);

    board->pieces[IDX(0, 7)] = board->pieces[IDX(7, 7)] = piece(C_BLACK, P_ROOK);
    board->pieces[IDX(1, 7)] = board->pieces[IDX(6, 7)] = piece(C_BLACK, P_BISHOP);
    board->pieces[IDX(2, 7)] = board->pieces[IDX(5, 7)] = piece(C_BLACK, P_KNIGHT);
    board->pieces[IDX(3, 7)] = piece(C_BLACK, P_QUEEN);
    board->pieces[IDX(4, 7)] = piece(C_BLACK, P_KING);

    for(uint8_t i = 0; i < 8; ++i) {
        board->pieces[IDX(i, 1)] = piece(C_WHITE, P_PAWN);
        board->pieces[IDX(i, 6)] = piece(C_BLACK, P_PAWN);
    }
}

void board_render_tile(board_t *board, piece_t piece, bool white_tile) {

}

void board_render(board_t *board, uint16_t x, uint16_t y) {
    
}

typedef struct chess_c {
    game_c super;
} chess_c;

typedef struct chess_t {
    game_t super;
    board_t board;
} chess_t;


int chess_run(game_t *game) {
    chess_t *self = (chess_t*)game;

    chess_move_iter_t moves;
    moves.stage.stage = 0;
    moves.current = (move_t){.from = IDX(4, 0)};
    moves.board = &self->board;
    moves.piece = self->board.pieces[IDX(4, 0)];
    
    while(chess_move_iter_drive(&moves)) {
        printf("Move to (%u, %u)\n", moves.current.to % 8, moves.current.to / 8);
    }

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
    board_new(&self->board);
}

game_t *chess(void) {
    chess_t *chess = malloc(sizeof(*chess));
    chess_new(chess);
    return (game_t*)chess;
}
