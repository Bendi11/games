set(SRC
    "src/bobj.c"
    "src/bobj/ops.c"
    "src/bobj/int.c"
    "src/bobj/iter.c"
    "src/bobj/str.c"
    "src/bobj/fn.c"
    "src/bobj/list.c"
    "src/bobj/iter/map.c"
    "src/bobj/log/log.c"
)

add_library(bobj STATIC ${SRC})
target_include_directories(bobj PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
