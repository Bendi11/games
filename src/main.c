#include <ncurses.h>
#include <stdio.h>

#include "engine.h"
#include "games.h"

int main(int argc, const char* argv[]) {
    ESCDELAY = 0;
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    engine_t engine = engine_new();
    engine_add(&engine, minesweeper());
    engine_run(&engine);
    endwin();

    drop((obj_i*)&engine);
    
    return 0;
}
