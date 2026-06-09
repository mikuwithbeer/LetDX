project(LetDD)

# --- Source and Output ---
set(SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SRC_FILE "${SRC_PATH}/main.zig")
set(OUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")

# --- Compiler Options ---
set(ZIG zig)
set(ZIG_FLAGS -O Debug)

# --- Watch Files ---
file(GLOB_RECURSE ZIG_SOURCES
    CONFIGURE_DEPENDS
    ${SRC_PATH}/*.zig
)

# --- Build Command ---
add_custom_command(
    OUTPUT ${OUT_FILE}
    COMMAND ${ZIG} build-exe ${SRC_FILE} ${ZIG_FLAGS} -femit-bin=${OUT_FILE}
    DEPENDS ${SRC_FILE}
    VERBATIM
)

# --- Add Target ---
add_custom_target(${PROJECT_NAME} ALL DEPENDS ${OUT_FILE})
