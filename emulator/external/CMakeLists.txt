






include(FetchContent)




find_package(Boost CONFIG COMPONENTS system program_options spirit QUIET)

if(NOT Boost_FOUND)
    message(STATUS "Boost not found. Fetching.")

    FetchContent_Declare(
        boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.zip
        DOWNLOAD_EXTRACT_TIMESTAMP FALSE
        CMAKE_ARGS -D"_WIN32_WINNT"="0x0A00"
    )

    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)

    FetchContent_MakeAvailable(boost)
endif()


FetchContent_Declare(
    elfio
    GIT_REPOSITORY https://github.com/serge1/ELFIO.git
    GIT_TAG Release_3.12
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(elfio)

FetchContent_Declare(
    abseil
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG 20250127.1
    GIT_SHALLOW TRUE
)

set(ABSL_BUILD_TESTING NO)
set(ABSL_USE_STD 20)


FetchContent_MakeAvailable(abseil)

add_library(avremul_external INTERFACE)
set_target_properties(avremul_external PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)

target_link_libraries(avremul_external INTERFACE Boost::spirit Boost::icl)