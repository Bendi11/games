set(SRC
    "src/bobj.c"
    "src/bobj/bint.c"
)

add_library(bobj STATIC ${SRC})
target_include_directories(bobj PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
