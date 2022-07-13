#include "bobj.h"

void drop(obj_t *obj) {
    vft_cast(obj_i, obj)->drop(obj);
}
