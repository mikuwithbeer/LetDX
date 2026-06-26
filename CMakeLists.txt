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
    # --- Warnings ---
    -Wall
    -Wextra
    -Wpedantic
    # --- Type Safety ---
    -Wconversion
    -Wshadow
    # --- Function Declarations ---
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Wold-style-definition
    -Werror=implicit-function-declaration
    # --- Format Strings ---
    -Wformat=2
    # --- Code Quality ---
    -Wundef
    -Wnull-dereference
    -Wpointer-arith
    -Wcast-align
    -Wcast-qual
    -Wbad-function-cast
    -Wvla
    -Wimplicit-fallthrough
    -Wunused-macros
)

target_compile_definitions(CompilationOpts INTERFACE
    $<$<CONFIG:Debug>:DEBUG>
)

# --- Child Directories ---
add_subdirectory(LetDB)
add_subdirectory(LetDD)
