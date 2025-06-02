cmake_minimum_required(VERSION 3.30)





project(avremu)


set(AVREMU_BUILD_TESTS YES CACHE BOOL "Defines whether to build tests for the emulator.")

if(${AVREMU_BUILD_TESTS})
    enable_testing()
endif()

add_subdirectory(emulator)
