project(LetDB
    VERSION 0.0.0
    LANGUAGES C
)

# --- Core Library ---
add_library(Let STATIC)
target_sources(Let PRIVATE
    src/network/request/decoder.c
    src/network/request/request.c
    src/network/response/encoder.c
    src/network/response/response.c
    src/network/server.c
    src/storage/crc.c
    src/storage/wal.c
    src/account.c
    src/cli.c
    src/error.c
    src/guard.c
    src/let.c
    src/log.c
    src/state.c
)

target_include_directories(Let PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Let PRIVATE CompilationOpts)

# --- Executable ---
add_executable(LetDB src/main.c)
target_link_libraries(LetDB PRIVATE Let)

# --- Testing ---
enable_testing()
add_subdirectory(tests)
