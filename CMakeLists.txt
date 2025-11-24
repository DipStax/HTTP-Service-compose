cmake_minimum_required(VERSION 3.27)  # 3.27+ has best Clang + reflection support

set(CMAKE_CXX_COMPILER /usr/local/bin/clang++)
set(CMAKE_C_COMPILER /usr/local/bin/clang)

project(HTTP_Service_Container
    VERSION 1.0.0
    LANGUAGES CXX
)

# Your exact manual flags — every single one
add_compile_options(
    -freflection
    -fexpansion-statements
    -fannotation-attributes
    -fparameter-reflection
    -stdlib=libc++
    -Wall -Wextra -pedantic
)

add_link_options(
    -stdlib=libc++
    -lc++
    -lc++abi
    -Wl,-rpath,/usr/local/lib
)

file(GLOB_RECURSE SRC_FILE
    include/**.hpp
    include/**.inl
    src/**.cpp
)

add_executable(server
    ${SRC_FILE}
)

set_target_properties(server PROPERTIES
    BUILD_RPATH_USE_ORIGIN TRUE
    INSTALL_RPATH "$ORIGIN:/usr/local/lib"
    LINK_FLAGS "-Wl,-rpath,/usr/local/lib"
)

target_compile_options(server PRIVATE -std=c++26)

target_include_directories(server PRIVATE include)