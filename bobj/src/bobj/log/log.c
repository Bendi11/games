#include "bobj/log/log.h"

static void blogger_drop(bobj_t *obj) {

}

vft_creator(
    blogger_c,
    blogger_c_impl,
    *self_class = (blogger_c){
        .super = (bobj_c){
            .name = "blogger",
            .size = sizeof(blogger_t) - sizeof(blogger_c*),
            .drop = blogger_drop,
        }
    };
)
