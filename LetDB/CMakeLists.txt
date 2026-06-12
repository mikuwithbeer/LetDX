project(LetDB
    VERSION 0.0.0
    LANGUAGES C
)

# --- Standards and Features ---
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# --- Executable ---
add_executable(LetDB)

# --- Source Files ---
target_sources(LetDB PRIVATE
    src/network/request/decoder.c
    src/network/request/request.c
    src/network/response.c
    src/network/server.c
    src/storage/crc.c
    src/storage/wal.c
    src/account.c
    src/cli.c
    src/error.c
    src/guard.c
    src/let.c
    src/main.c
    src/state.c
)

# --- Include Directories ---
target_include_directories(LetDB PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# --- Apply Compilation Options ---
target_link_libraries(LetDB PRIVATE CompilationOpts)
