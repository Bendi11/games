#pragma once

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

/** Signature of a function to run when the given window is resized */
typedef void (*term_resize_cb_t)(uint16_t w, uint16_t h);

/** Execute the given callback function when the terminal is resized */
void term_on_resize(term_resize_cb_t cb);

/** Set terminal attributes for the next characters to be drawn */
void term_attr(term_attr_t);

/** Read a character from stdin */
int term_readch(void);

/** Read a character from the terminal with timeout */
int term_readch_timeout(uint32_t);

/** Clear the terminal buffer of any characters */
void term_clear(void);

/** Move the cursor to (x, y) */
void term_cursor(uint16_t x, uint16_t y);

/** Set the terminal foreground color to c */
void term_fg(term_color_t c);
/** Set the terminal background color to c */
void term_bg(term_color_t c);

/** Set wether to show the cursor onscreen or not */
void term_cursorshow(bool);
