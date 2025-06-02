








file(GLOB sources ./src/*.cxx)
file(GLOB headers ./headers/*.hxx)


include(FetchContent)

FetchContent_Declare(
    catchtwo
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1
    GIT_SHALLOW YES
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(catchtwo)

FetchContent_Declare(
    trompeloeil
    GIT_REPOSITORY https://github.com/rollbear/trompeloeil.git
    GIT_TAG v49
    GIT_SHALLOW YES
    GIT_PROGRESS YES
)

FetchContent_MakeAvailable(trompeloeil)

enable_testing()

include(CTest)
include(Catch)

add_executable(avremul_tests)

target_sources(avremul_tests PRIVATE ${sources})
target_sources(avremul_tests PRIVATE FILE_SET header_set TYPE "HEADERS" FILES ${headers})

set_target_properties(avremul_tests PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)

target_link_libraries(avremul_tests PRIVATE Catch2::Catch2WithMain trompeloeil::trompeloeil avremu::emuimpl)
target_include_directories(avremul_tests PRIVATE ./headers)


catch_discover_tests(avremul_tests)