#include <stdio.h>
#include <locale.h>

#include "engine.h"
#include "games.h"
#include "term/term.h"

int main(int argc, const char* argv[]) {
    setlocale(LC_ALL, "");
    term_start();
    term_raw();


    engine_t engine;
    engine_new(&engine);
    engine_add(&engine, minesweeper());
    engine_run(&engine);
    
    bobj_drop((bobj_t*)&engine);
    return 0;
}
