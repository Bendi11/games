#include "write.h"

vft_creator(
    bwriter_c,
    bwriter_c_impl,
    (bwriter_c){
        .super = (bobj_c){
            .name = "bwriter",
            .traits = s_btraitlist(),
            .drop = bobj_default_drop,
            .size = 0,
        },
        .write = (void*)bobj_virtual,
    }
)
