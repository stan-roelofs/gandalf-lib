cmake_minimum_required(VERSION 3.11)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(SOURCES
  src/blargg_test.cpp
  src/cartridge_test.cpp
  src/mooneye_test.cpp
  src/ppu_test.cpp
  src/resource_helper.h
  src/resource_helper.cpp
  src/serial_test.cpp
  src/serialization_test.cpp
  src/timer_test.cpp
  src/wram_test.cpp
)

add_executable(gandalf-lib-test ${SOURCES})
target_link_libraries(gandalf-lib-test gandalf-lib gtest_main)

string(CONCAT RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/resources)
target_compile_definitions(gandalf-lib-test PRIVATE RESOURCE_PATH=\"${RESOURCE_PATH}\")

include(GoogleTest)
gtest_discover_tests(gandalf-lib-test)