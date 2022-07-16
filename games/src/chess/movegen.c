#include "chess.h"

/** \return true If moves->current was set to a new move or false if there are no more moves to generate */
__attribute__((hot))
bool chess_move_iter_drive(chess_move_iter_t *moves) {
    moves->current = (move_t){
        .capture = false,
        .promote = false,
        .enpassant = false,
        .castle = false,
        .from_rank = moves->from / 8,
        .from_file = moves->from % 8,
    };
    square_t from = moves->from;
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

