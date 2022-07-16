#include <stdio.h>
#include <locale.h>

#include "bobj/int.h"
#include "bobj/fn.h"
#include "bobj/list.h"
#include "bobj/str.h"
#include "engine.h"
#include "games.h"
#include "term/term.h"

static void call(blist_t* args, bobj_t *ores) {
    bint_t *lhs = (bint_t*)blist_at(args, 0);
    bint_t *rhs = (bint_t*)blist_at(args, 1);
    vft_cast(bint_c, lhs)->add.add((bobj_t*)lhs, (bobj_t*)rhs, ores);
}

static bool check(blist_t* args) {
    return true;
}

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
