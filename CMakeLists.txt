cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_COMPILER /usr/local/bin/clang++)
set(CMAKE_C_COMPILER /usr/local/bin/clang)

project(HTTP_Service_Container
    VERSION 1.0.0
    LANGUAGES CXX
)

add_compile_options(
    -std=c++26
    -freflection
    -fexpansion-statements
    -fannotation-attributes
    -fparameter-reflection
    -ftemplate-backtrace-limit=0
    -Wall -Wextra -pedantic
)

add_link_options(
    -stdlib=libc++
)

file(GLOB_RECURSE SRC_FILE
    include/**.hpp
    include/**.inl
    src/HSC/**.cpp
)

add_library(HSC
    ${SRC_FILE}
)

target_include_directories(HSC PUBLIC include)

add_executable(hsc_test
    src/main.cpp
)

target_link_libraries(hsc_test PUBLIC HSC)
