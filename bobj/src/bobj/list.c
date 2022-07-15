#include "list.h"
#include "bobj.h"

#include <malloc.h>

vft_creator(
    blist_c,
    blist_c_impl,
    (blist_c){
        .super = (bobj_c){
            .name = "blist",
            .drop = bobj_virtual,
            .size = 0,
        },
        .at = (void*)bobj_virtual,
        .len = (void*)bobj_virtual,
    }
)
