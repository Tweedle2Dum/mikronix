cmake_minimum_required(VERSION 3.20)

project(mikronix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(kernel
    kernel/src/main.cpp
)

target_include_directories(kernel PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_options(kernel PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -ffreestanding
    -mno-red-zone
)

target_link_options(kernel PRIVATE
    "-T${CMAKE_SOURCE_DIR}/linkers/kernel.ld"
)
