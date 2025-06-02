






add_subdirectory(external)
add_subdirectory(src)

if(${AVREMU_BUILD_TESTS})
    add_subdirectory(tests)
endif()