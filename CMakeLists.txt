cmake_minimum_required(VERSION 4.3)

project(LetDB
    VERSION 0.0.0
    LANGUAGES C)

# --- Standards and Features ---
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# --- Build Configuration ---
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")

# --- Source Files ---
set(SOURCES
    src/account.c
    src/main.c
    src/network.c
    src/state.c
)

add_executable(LetDB ${SOURCES})

# --- Include Directories ---
target_include_directories(LetDB PRIVATE include)

# --- Compilation Options ---
target_compile_options(LetDB PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wshadow
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Werror=implicit-function-declaration
    -Wformat=2
)
