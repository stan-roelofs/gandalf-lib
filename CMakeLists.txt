cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
 
project(gandalf-lib VERSION 0.9.0)

option(GANDALF_BUILD_TESTS "Build tests" OFF)
option(GANDALF_BUILD_EXAMPLES "Build examples" OFF)

set(HEADERS
    include/gandalf/apu.h
    include/gandalf/memory.h
    include/gandalf/cartridge.h
    include/gandalf/cpu.h
    include/gandalf/cpu_registers.h
    include/gandalf/constants.h
    include/gandalf/dma.h
    include/gandalf/exception.h
    include/gandalf/gameboy.h
    include/gandalf/hdma.h
    include/gandalf/hram.h
    include/gandalf/io.h
    include/gandalf/joypad.h
    include/gandalf/lcd.h
    include/gandalf/mbc.h
    include/gandalf/model.h
    include/gandalf/ppu.h
    include/gandalf/serial.h
    include/gandalf/sound/frame_sequencer.h
    include/gandalf/sound/sound_channel.h
    include/gandalf/timer.h
    include/gandalf/types.h
    include/gandalf/util.h
    include/gandalf/wram.h
    src/bootrom.h
    src/cartridge/mbc1.h
    src/cartridge/mbc3.h
    src/cartridge/mbc5.h
    src/cartridge/rom_only.h
    src/sound/frequency_sweep_unit.h
    src/sound/length_counter.h
    src/sound/noise_channel.h
    src/sound/square_wave_channel.h
    src/sound/volume_envelope.h
    src/sound/wave_channel.h
)

set(SOURCES
    src/apu.cpp
    src/bootrom.cpp
    src/memory.cpp
    src/cartridge.cpp
    src/cartridge/mbc.cpp
    src/cartridge/mbc1.cpp
    src/cartridge/mbc3.cpp
    src/cartridge/mbc5.cpp
    src/cartridge/rom_only.cpp
    src/cpu.cpp
    src/dma.cpp
    src/gameboy.cpp
    src/hdma.cpp
    src/hram.cpp
    src/io.cpp
    src/joypad.cpp
    src/lcd.cpp
    src/model.cpp
    src/ppu.cpp
    src/serial.cpp
    src/sound/frame_sequencer.cpp
    src/sound/frequency_sweep_unit.cpp
    src/sound/length_counter.cpp
    src/sound/noise_channel.cpp
    src/sound/sound_channel.cpp
    src/sound/square_wave_channel.cpp
    src/sound/volume_envelope.cpp
    src/sound/wave_channel.cpp
    src/timer.cpp
    src/wram.cpp
)

add_library(gandalf-lib ${SOURCES} ${HEADERS})

if(MSVC)
  target_compile_options(gandalf-lib PRIVATE /W4 /WX)
else()
  target_compile_options(gandalf-lib PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(gandalf-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(GANDALF_BUILD_TESTS)
  enable_testing()
  add_subdirectory(external/googletest)
  add_subdirectory(tests)
endif()

if (GANDALF_BUILD_EXAMPLES)
  set(SDL_STATIC ON CACHE BOOL "" FORCE)
  set(SDL_SHARED OFF CACHE BOOL "" FORCE)
  add_subdirectory(external/SDL)  
  add_subdirectory(examples)
endif()