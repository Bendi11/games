#include "bobj.h"
#include "bobj/int.h"
#include "term.h"

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>

#define CSI "\x1b["

static void term_resize_cb_call(bfn_t *fn, blist_t *args, bobj_t *res) {
    vft_cast(term_resize_cb_c, fn)->on_resize(
        ((bu16_t*)blist_at(args, 0))->val, 
        ((bu16_t*)blist_at(args, 1))->val
    );
}

static bool term_resize_cb_check(bfn_t *fn, blist_t *args) {
    return blist_len(args) == 2 &&
        bobj_instanceof(bu16_c_impl(), blist_at(args, 0)) &&
        bobj_instanceof(bu16_c_impl(), blist_at(args, 1));
}

static void term_resize_cb_drop(bobj_t* obj) {}

void term_resize_cb_new(term_resize_cb_t *cb) {
    vft_cast(term_resize_cb_c, cb) = term_resize_cb_c_impl();
    cb->super.return_ty = NULL;
}

vft_creator(
    term_resize_cb_c,
    term_resize_cb_c_impl,
    (term_resize_cb_c){
        .on_resize = (void*)bobj_virtual,
        .super = (bfn_c) {
            .super = (bobj_c){
                .name = "term_resize_cb",
                .drop = term_resize_cb_drop,
                .size = sizeof(term_resize_cb_t) - sizeof(term_resize_cb_c*),
            },
            .call = term_resize_cb_call,
            .typecheck = term_resize_cb_check,
        }
    }
)

typedef struct resize_cb_list_s {
    term_resize_cb_t *cb;
    struct resize_cb_list_s *next;
} resize_cb_list_t;

static struct termios initial;
static resize_cb_list_t *resize_cb_list = NULL;
static uint16_t width = 0;
static uint16_t height = 0;

void term_reset(void) {
    tcsetattr(1, TCSANOW, &initial);
    fputs(CSI "?1049l", stdout);
}

static void die(int i) {
    exit(i);
}

static void resize(int i) {
    struct winsize size;
    ioctl(1, TIOCGWINSZ, &size);
    width = size.ws_col;
    height = size.ws_row;
    resize_cb_list_t *cb = resize_cb_list;
    while(cb != NULL) {
        vft_cast(term_resize_cb_c, &cb->cb)->on_resize(width, height);
        cb = cb->next;
    }
}

void term_start() {
    fputs(CSI "?1049h", stdout);
    struct termios term;
    tcgetattr(1, &term);
    initial = term;
    atexit(term_reset);
    signal(SIGTERM, die);
    signal(SIGINT, die);
    signal(SIGWINCH, resize);
    resize(0);
}

void term_raw() {
    //setvbuf(stdout, NULL, _IONBF, 0);
    struct termios term;
    tcgetattr(1, &term);
    term.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(1, TCSANOW, &term);
}

void term_attr(term_attr_t attrs) {
    fprintf(
        stdout,
        CSI "%s%s%s%s%sm",
        attrs.reset_style ? "22" : "",
        attrs.bold ? ";1" : "",
        attrs.italic ? ";3" : "",
        attrs.underline ? ";4" : "",
        attrs.reset_all ? "0" : ""
    );
}

void term_refresh() {
    fflush(stdout);
}

uint16_t term_width() { return width; }
uint16_t term_height() { return height; }

void term_on_resize(term_resize_cb_t *cb) {
    resize_cb_list_t **tail = &resize_cb_list;
    while(*tail != NULL) {
        tail = &(*tail)->next;
    }
    *tail = malloc(sizeof(**tail));
    (*tail)->cb = cb;
    (*tail)->next = NULL;
}

/*void term_timeout(uint16_t timeout) {
    struct termios term;
    tcgetattr(1, &term);
    if(timeout != (uint16_t)-1) {
        term.c_lflag &= ~ICANON;
        term.c_cc[VTIME] = timeout / 10;
    } else {
        term.c_cc[VTIME] = -1;
    }
    tcsetattr(1, TCSANOW, &term);
}*/

static inline int postprocess(int c) {
    if(c == '\x1b') {
        if(fgetc(stdin) == '[') {
            int arrow = fgetc(stdin);
            switch(arrow) {
                case 'A': return TERM_UP;
                case 'B': return TERM_DOWN;
                case 'C': return TERM_RIGHT;
                case 'D': return TERM_LEFT;
            }
        }
    }
    return c;
}

int term_readch() {
    return postprocess(fgetc(stdin));
}

int term_readch_timeout(uint32_t timeout) {
    char c;
    struct timeval time = {0, timeout * 1000};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(1, &fds);
    int bytes_ready;
    if((bytes_ready = select(1 + 1, &fds, NULL, NULL, &time)) > 0) {
        return postprocess(fgetc(stdin));
    } else {
        return -1;
    }
}

void term_clear() {
    fputs(CSI "2J", stdout);
}

void term_cursor(uint16_t x, uint16_t y) {
    fprintf(stdout, CSI "%u;%uf", y + 1, x + 1);
}
void term_fg(term_color_t c) {
    fprintf(stdout, CSI "38;2;%u;%u;%um", c.r, c.g, c.b);
}
void term_bg(term_color_t c) {
    fprintf(stdout, CSI "48;2;%u;%u;%um", c.r, c.g, c.b);
}

void term_cursorshow(bool show) {
    if(show) {
        fputs(CSI "?25h", stdout);
    } else {
        fputs(CSI "?25l", stdout);
    }
}
