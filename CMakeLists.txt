cmake_minimum_required(VERSION 4.0)

project(LetDX)

# --- Standards and Features ---
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# --- Global Compilation Options ---
add_library(CompilationOpts INTERFACE)
add_library(LetDX::CompilationOpts ALIAS CompilationOpts)

target_compile_options(CompilationOpts INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wshadow
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Werror=implicit-function-declaration
    -Wformat=2
    -Wundef
    -Wunreachable-code
)

target_compile_definitions(CompilationOpts INTERFACE
    $<$<CONFIG:Debug>:DEBUG>
)

# --- Child Directories ---
add_subdirectory(LetDB)
add_subdirectory(LetDD)
