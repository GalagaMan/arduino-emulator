







file(GLOB_RECURSE sources ./src/*.cxx)
file(GLOB_RECURSE headers ./headers/*.hxx)

list(FILTER sources EXCLUDE REGEX ".*main.cxx$")

add_library(emuimpl STATIC)
add_library("${PROJECT_NAME}::emuimpl" ALIAS emuimpl)


target_sources(emuimpl PRIVATE ${sources})
target_sources(emuimpl PUBLIC FILE_SET header TYPE HEADERS FILES ${headers})


set_target_properties(emuimpl PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)

target_include_directories(emuimpl PUBLIC ./headers)
target_link_libraries(emuimpl PUBLIC avremul_external)

add_executable(avremul)

target_sources(avremul PRIVATE ./src/main.cxx)

target_compile_options(avremul PUBLIC -march=znver3 -mcx16)

set_target_properties(avremul PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)

target_link_libraries(avremul PRIVATE "${PROJECT_NAME}::emuimpl")

target_compile_options(avremul PRIVATE -mcx16 -fvectorize -funroll-loops)
