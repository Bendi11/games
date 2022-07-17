#include "bobj/iter.h"
#include "bobj/str.h"
#include "games.h"
#include "term/term.h"
#include "chess/chess.h"

#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>

const char * piece_glyphs[2][6] = {
#ifdef CHESS_UNICODE
    {
        "\u265f",
        "\u265e",
        "\u265d",
        "\u265c",
        "\u265b",
        "\u265a",
    },
    {
        "\u2659",
        "\u2658",
        "\u2657",
        "\u2656",
        "\u2655",
        "\u2654",
    },
#else
    {
        "P",
        "N",
        "B",
        "R",
        "Q",
        "K",
    },
    {
        "p",
        "n",
        "b",
        "r",
        "q",
        "k",
    },
#endif
};

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

move_t move_default = {
    .from_file = 8,
    .from_rank = 8,
    .from_type = P_PAWN,
    .capture = false,
    .castle = false,
    .enpassant = false,
    .promote = false,
    .to = 0,
};

/** Attempt to parse a chess move in algebraic notation from the given string */
static const char * move_parse(move_t *move, const char *str) {
    *move = move_default;
    move->from_type = P_PAWN;
    size_t len = strcspn(str, " \n\t\r");
    if(strncmp(str, "O-O", len) == 0) {
        move->castle = true;
        move->castle_side = CASTLE_KINGSIDE;
        return NULL;
    } else if(strncmp(str, "O-O-O", len) == 0) {
        move->castle = true;
        move->castle_side = CASTLE_QUEENSIDE;
        return NULL;
    }
    if(len < 2) return "no destination tile";
    uint8_t to_rank = str[len - 1] - '1';
    if(to_rank >= 8) return "destination rank is invalid";
    uint8_t to_file = tolower(str[len - 2]) - 'a';
    if(to_file >= 8) return "destination file is invalid";
    move->to = IDX(to_file, to_rank);
    len = len - 2;
    while(len > 0) {
        len -= 1;
        char c = tolower(str[len]);
        switch(c) {
            case '?':
            case '!': break;
            case 'x': move->capture = true; break;
            case 'a': 
            case 'c'...'h': move->from_file = c - 'a'; break;
            case '1'...'8': move->from_rank = c - '1'; break;

            case 'p': move->from_type = P_PAWN; break;
            case 'n': move->from_type = P_KNIGHT; break;
            case 'b': {
                if(move->from_type == P_BISHOP) {
                    move->from_file = 1;
                } else {
                    move->from_type = P_BISHOP;
                }
            } break;
            case 'r': move->from_type = P_ROOK; break;
            case 'q': move->from_type = P_QUEEN; break;
            case 'k': move->from_type = P_KING; break;
            default: return "Invalid character";
        } 
    }

    return NULL;
}

bstr_t move_tostr(move_t *move) {
    bstr_t string = s_bstr();
    if(move->from_type != P_PAWN) { bstr_appendcstr(&string, piece_glyphs[C_WHITE][move->from_type]); }
    if(move->from_file < 8) { bstr_appendc(&string, move->from_file + 'a'); }
    if(move->from_rank < 8) { bstr_appendc(&string, move->from_rank + '1'); }
    if(move->capture) { bstr_appendc(&string, 'x'); }

    bstr_appendc(&string, (move->to % 8) + 'a');
    bstr_appendc(&string, (move->to / 8) + '1');

    return string;
}

static term_color_t white_tile_c = (term_color_t){230, 230, 230};
static term_color_t black_tile_c = {20, 20, 20};

void board_render_tile(board_t *board, uint16_t x, uint16_t y, piece_t piece, bool white_tile) {
    term_cursor(x, y);
    term_bg(white_tile ? white_tile_c : black_tile_c);
    fputs("       ", stdout);
    term_cursor(x, y + 1);
    if(piece.exists) {
        term_fg(white_tile ? black_tile_c : white_tile_c);
        fprintf(stdout, "   %s   ", piece_glyphs[piece.color][piece.type]);
    } else {
        fputs("       ", stdout);
    }
    term_cursor(x, y + 2);
    fputs("       ", stdout);
}

void board_render(board_t *board, uint16_t x, uint16_t y) {
    bool white = true;
    uint8_t row = 0;
    for(uint8_t i = 0; i < 64; ++i) {
        if(i / 8 > row) {
            row = i / 8;
            white = (row % 2 == 0) ? true : false;
        }
        board_render_tile(board, x + (i % 8) * 7, y + ((7 - (i / 8)) * 3), board->pieces[i], white);
        white = !white;
    }
    term_refresh();
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
    moves.from = IDX(4, 0);
    moves.board = &self->board;
    moves.piece = self->board.pieces[IDX(4, 0)];

    bstr_t movestr = s_bstr();
    for(;;) {
        int in = '\0';
        while(in != '\n') {
            in = term_readch();
            if(in == 127) {
                bstr_popc(&movestr);
            } else if(in != '\n') {
                bstr_appendc(&movestr, in);
            }
            term_clear_line();
            putchar('\r');
            fwrite(movestr.data, sizeof(char), movestr.len, stdout);
            term_refresh();
        }
        move_t move;
        const char *error = move_parse(&move, bstr_cstr(&movestr));
        if(error) {
            term_clear_line();
            printf("\rError: %s\n", error);
            bstr_clear(&movestr);
            term_refresh();
            usleep(1000000);
        } else {
            bstr_clear(&movestr);
            term_clear_line();
            chess_make_move(&self->board, move);
            board_render(&self->board, term_width() / 2 - 35, 0);
            term_cursor(0, term_height() - 1);
            term_refresh();
        }
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
