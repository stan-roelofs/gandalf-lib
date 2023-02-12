project(example-sdl)

set(HEADERS
    src/audio_handler.h
)

set(SOURCES
    src/audio_handler.cpp
    src/main.cpp
)

add_executable(example-sdl ${HEADERS} ${SOURCES})
target_link_libraries(example-sdl gandalf-lib SDL2main SDL2-static)