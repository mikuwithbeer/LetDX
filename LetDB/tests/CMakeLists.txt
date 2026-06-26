# --- Test Sources ---
set(TEST_SOURCES
    network/request.c
    network/response.c
    network/server.c
    storage/crc.c
    account.c
    guard.c
    state.c
)

# --- Test Executables ---
foreach (SourceFile IN LISTS TEST_SOURCES)
    get_filename_component(BaseName ${SourceFile} NAME_WE)
    set(TargetName "test_${BaseName}")

    add_executable(${TargetName} ${SourceFile})

    target_compile_options(${TargetName} PRIVATE "-UNDEBUG")
    target_link_libraries(${TargetName} PRIVATE LetDB::Let)

    add_test(NAME ${TargetName} COMMAND ${TargetName})
endforeach ()
