set(SRC
    "src/bobj.c"
    "src/bobj/bint.c"
    "src/bobj/iter.c"
    "src/bobj/array.c"
    "src/bobj/str.c"
    "src/bobj/iter/map.c"
)

add_library(bobj STATIC ${SRC})
target_include_directories(bobj PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
