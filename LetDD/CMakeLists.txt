project(LetDD LANGUAGES NONE)

find_program(GO_EXECUTABLE go REQUIRED)

# --- Executable Name ---
set(ExecutableName "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")

# --- Source Files ---
file(GLOB_RECURSE Sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.go"
    "${CMAKE_CURRENT_SOURCE_DIR}/go.mod"
    "${CMAKE_CURRENT_SOURCE_DIR}/go.sum"
)

# --- Build Executable ---
add_custom_command(
    OUTPUT "${ExecutableName}"
    COMMAND ${CMAKE_COMMAND} -E env GO111MODULE=on CGO_ENABLED=0
    ${GO_EXECUTABLE} build -o ${ExecutableName} .
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS ${Sources}
    VERBATIM
)

add_custom_target(LetDD ALL DEPENDS "${ExecutableName}")
