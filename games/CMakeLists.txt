set(SRC
    "src/main.c"
    "src/mines.c"
    "src/engine.c"
    "src/term/ansi.c"
    "src/chess.c"
)

set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)

add_executable(games ${SRC})
target_compile_options(games PRIVATE ${CURSES_CFLAGS})
target_include_directories(games PUBLIC ${CURSES_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/src")
#target_precompile_headers(games PUBLIC "${CURSES_INCLUDE_DIR}/ncurses.h")
target_link_libraries(games ${CURSES_LIBRARIES} ncursesw menu bobj)
