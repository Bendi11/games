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
} direction_t;

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

#define INVALID_SQUARE ((uint8_t)255)
#define IDX(x, y) (((y) * 8) + (x))
#define VALID(x) ((uint8_t)(x) < 64)

typedef struct board_t {
    piece_t pieces[64];
    square_t enpassant;
    uint8_t turn;
} board_t;

typedef struct move_t {
    square_t from : 4;
    square_t to : 4;
    bool promote : 1;
    bool capture : 1;
    bool enpassant : 1;
    bool castle : 1;
    union {
        piece_t promoted;
        piece_t captured;
        uint8_t castle_side;
    };
} move_t;

typedef struct {
    union {
        uint8_t stage;
        struct {
            uint8_t n : 5;
            uint8_t direction : 3;
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
        .from = moves->current.from
    };
    square_t from = moves->current.from;
    switch(moves->piece.type) {
        case P_PAWN: {
            square_t to;
            switch(moves->stage.stage) {
                case 0: {
                    moves->stage.stage += 1;
                    if(!moves->piece.moved) {
                        square_t between = moves->piece.color ? square_up(from, 1) : square_down(from, 1);
                        to = moves->piece.color ? square_up(from, 2) : square_down(from, 2);
                        if(!moves->board->pieces[to].exists && !moves->board->pieces[between].exists) {
                            moves->current.to = to;
                            return true;
                        }
                    }
                }
                case 1: {
                    moves->stage.stage += 1;
                    if((moves->piece.color ? (from / 8 + 1) : (from / 8 - 1)) < 8) {
                        to = moves->piece.color ? square_up(from, 1) : square_down(from, 1);
                        if(!moves->board->pieces[to].exists) {
                            moves->current.from = from;
                            moves->current.to = to;
                            break;
                        }
                    }
                }
            }
        } break;
    }

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
