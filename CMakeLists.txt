cmake_minimum_required(VERSION 3.23)

project(
    games
    LANGUAGES C
)

set(SRC
    "src/main.c"
    "src/mines.c"
    "src/engine.c"
)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)

add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${CURSES_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_precompile_headers(${PROJECT_NAME} PUBLIC "${CURSES_INCLUDE_DIR}/ncurses.h")
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES} menu)
