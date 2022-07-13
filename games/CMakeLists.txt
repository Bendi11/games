set(SRC
    "src/main.c"
    "src/mines.c"
    "src/engine.c"
)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)

add_executable(games ${SRC})
target_include_directories(games PUBLIC ${CURSES_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_precompile_headers(games PUBLIC "${CURSES_INCLUDE_DIR}/ncurses.h")
target_link_libraries(games ${CURSES_LIBRARIES} menu bobj)
