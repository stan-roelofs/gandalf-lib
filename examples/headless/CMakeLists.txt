project(example-headless)

set(HEADERS
)

set(SOURCES
    src/main.cpp
)

add_executable(example-headless ${HEADERS} ${SOURCES})
target_link_libraries(example-headless gandalf-lib)