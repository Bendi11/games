#pragma once

#include "bobj/fn.h"
#include <stdint.h>
#include <stdbool.h>

#define TERM_UP (38)
#define TERM_DOWN (40)
#define TERM_LEFT (37)
#define TERM_RIGHT (39)

typedef struct term_attr_s {
    bool bold : 1;
    bool underline : 1;
    bool italic : 1;
    bool reset_style : 1;
    bool reset_all : 1;
} term_attr_t;

/** An RGB color structure */
typedef struct term_color_s {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} term_color_t;

/** Initialize all state to control the terminal */
void term_start(void);

/** Write all buffered output to stdout */
void term_refresh(void);

/** Disable input buffering */
void term_raw(void);

/** Reset the terminal's flags */
void term_reset(void);

/** Get number of columns in this terminal */
uint16_t term_width(void);
/** Get the number of lines in this terminal */
uint16_t term_height(void);

typedef struct term_resize_cb_t term_resize_cb_t;
typedef struct {
    bfn_c super;
    void (*on_resize)(uint16_t, uint16_t);
} term_resize_cb_c;

extern term_resize_cb_c* (*term_resize_cb_c_impl)(void);

typedef struct term_resize_cb_t {
    bfn_t super;
} term_resize_cb_t;

void term_resize_cb_new(term_resize_cb_t *cb);

/** Execute the given callback function when the terminal is resized */
void term_on_resize(term_resize_cb_t *cb);

/** Set terminal attributes for the next characters to be drawn */
void term_attr(term_attr_t);

/** Read a character from stdin */
int term_readch(void);

/** Read a character from the terminal with timeout */
int term_readch_timeout(uint32_t);

/** Clear the terminal buffer of any characters */
void term_clear(void);

/** Clear the current line of all characters and formatting */
void term_clear_line(void);

/** Move the cursor to (x, y) */
void term_cursor(uint16_t x, uint16_t y);

void term_cursorup(uint16_t n);
void term_cursordown(uint16_t n);
void term_cursorleft(uint16_t n);
void term_cursorright(uint16_t n);

/** Set the terminal foreground color to c */
void term_fg(term_color_t c);
/** Set the terminal background color to c */
void term_bg(term_color_t c);

/** Set wether to show the cursor onscreen or not */
void term_cursorshow(bool);
