cmake_minimum_required(VERSION 3.11)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(SOURCES
  src/blargg_test.cpp
  src/cartridge_test.cpp
  src/mooneye_test.cpp
  src/resource_helper.h
  src/resource_helper.cpp
)

add_executable(libgb_test ${SOURCES})
target_link_libraries(libgb_test libgb gtest_main)

string(CONCAT RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/resources)
target_compile_definitions(libgb_test PRIVATE RESOURCE_PATH=\"${RESOURCE_PATH}\")

include(GoogleTest)
gtest_discover_tests(libgb_test)