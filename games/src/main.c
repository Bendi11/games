#include <stdio.h>
#include <locale.h>

#include "bobj/int.h"
#include "bobj/fn.h"
#include "bobj/list.h"
#include "bobj/str.h"
#include "engine.h"
#include "games.h"
#include "term/term.h"

void test(int a[]) {

}

int main(int argc, const char* argv[]) {
    setlocale(LC_ALL, "");
    term_start();
    term_raw();
    engine_t engine;
    engine_new(&engine);
    engine_add(&engine, minesweeper());
    engine_add(&engine, chess());
    engine_run(&engine);
    
    bobj_drop((bobj_t*)&engine);
    
    term_reset();
    return 0;
}
