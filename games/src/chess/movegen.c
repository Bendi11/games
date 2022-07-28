#include "bobj.h"
#include "chess.h"
#include "term/term.h"

#include <malloc.h>

/** \return true If moves->current was set to a new move or false if there are no more moves to generate */
__attribute__((hot))
bool chess_move_iter_drive(chess_move_iter_t *moves) {
    moves->current = move_default;
    moves->current.from_rank = moves->from / 8;
    moves->current.from_file = moves->from % 8;
    moves->current.from_type = moves->piece.type;
    square_t from = moves->from;
    switch(moves->current.from_type) {
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
                    to = square_left(up, 1);
                    if(up_valid && ((uint8_t)(from % 8 - 1) < 8) && moves->board->pieces[to].exists && moves->board->pieces[to].color != moves->piece.color) {
                        moves->current.to = to;
                        moves->current.capture = true;
                        return true;
                    }
                }
                case 3: {
                    moves->stage.stage += 1;
                    to = square_right(up, 1);
                    if(up_valid && ((uint8_t)(from % 8 + 1) < 8) && moves->board->pieces[to].exists && moves->board->pieces[to].color != moves->piece.color) {
                        moves->current.to = to;
                        moves->current.capture = true;
                        return true;
                    }
                }
            }
        } return false;
        case P_KNIGHT: {
            square_t to;
#define ATTACK(n, dx, dy) \
                case n: { \
                    moves->stage.stage += 1;   \
                    uint8_t x = (from % 8) dx; \
                    uint8_t y = (from / 8) dy; \
                    to = IDX(x, y);            \
                    if(                        \
                        x < (uint8_t)8 &&      \
                        y < (uint8_t)8 &&      \
                        (!moves->board->pieces[to].exists || moves->board->pieces[to].color != moves->piece.color) \
                    ) {                                     \
                        moves->current.to = to;             \
                        moves->current.capture = moves->board->pieces[to].exists;      \
                        return true;                        \
                    }                                       \
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
        } return false;
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
                            n = 1;
                        }
                    } else {
                        moves->current.to = to;
                        moves->stage.direction = direction;
                        moves->stage.n = n + 1;
                        return true;
                    }
                } else {
                    direction += step;
                    n = 1;
                }
           }
        } return false;
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
        } return false;
    }
#undef ATTACK
    return false;
}

bobj_t *chess_move_iter_next_impl(biter_t *iter) {
    chess_move_iter_t *self = (chess_move_iter_t*)iter;
    return NULL;
}

static inline bool move_eq(move_t lhs, move_t rhs) {
    return 
        lhs.capture == rhs.capture &&
        lhs.promote == rhs.promote &&
        lhs.enpassant == rhs.enpassant &&
        lhs.castle == rhs.castle &&
        lhs.from_type == rhs.from_type &&
        lhs.to == rhs.to;
}

vft_creator(
    chess_move_iter_c,
    chess_move_iter_c_impl,
    *self_class = (chess_move_iter_c){
        .super = (biter_c){

        }
    };
)

void chess_move_iter_new(chess_move_iter_t *iter, board_t *board, square_t from) {
    vft_cast(chess_move_iter_c, iter) = chess_move_iter_c_impl();
    iter->board = board;
    iter->current = move_default;
    iter->from = from;
    iter->piece = board->pieces[from];
    iter->stage = 
        (iter->piece.type == P_QUEEN || iter->piece.type == P_ROOK) ? (movegen_stage_t){.direction = D_N, .n = 1} :
            (iter->piece.type == P_BISHOP) ?
                (movegen_stage_t){.direction = D_NE, .n = 1} :
                (movegen_stage_t){.stage = 0};
}

square_t chess_move_valid(board_t *board, move_t move) {
    chess_move_iter_t moves;
    if(move.from_file < 8 && move.from_rank < 8) {
        square_t idx = IDX(move.from_file, move.from_rank);
        chess_move_iter_new(&moves, board, idx);
        while(chess_move_iter_drive(&moves)) {
            if(move_eq(moves.current, move)) return idx;
        }
        return INVALID_SQUARE;
    }

    for(square_t i = 0; i < 64; ++i) {
        if(
            board->pieces[i].color == board->turn &&
            board->pieces[i].exists &&
            board->pieces[i].type == move.from_type &&
            (move.from_file >= 8 || move.from_file == (i % 8)) &&
            (move.from_rank >= 8 || move.from_rank == (i / 8))
        ) {
            chess_move_iter_new(&moves, board, i);
            while(chess_move_iter_drive(&moves)) {
                /*printf(
                    "%s at (%u, %u) can move to (%u, %u)\n",
                    piece_glyphs[C_WHITE][board->pieces[i].type],
                    i % 8,
                    i / 8,
                    moves.current.to % 8,
                    moves.current.to / 8
                );*/
                if(move_eq(moves.current, move)) return i;
            }
        }
    }
    return INVALID_SQUARE;
}

bool chess_make_move(board_t *board, move_t move) {
    square_t from = chess_move_valid(board, move);
    if(from == INVALID_SQUARE) return false;
    board->pieces[move.to] = board->pieces[from];
    board->pieces[move.to].exists = true;
    board->pieces[from].exists = false;
    board->turn = !board->turn;
    return true;
}
