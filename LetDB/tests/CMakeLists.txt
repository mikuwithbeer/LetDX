# --- network/request.c ---
add_executable(TestRequest network/request.c)

target_compile_options(TestRequest PRIVATE "-UNDEBUG")
target_link_libraries(TestRequest PRIVATE Let)

add_test(NAME LetTestRequest COMMAND TestRequest)

# --- network/response.c ---
add_executable(TestResponse network/response.c)

target_compile_options(TestResponse PRIVATE "-UNDEBUG")
target_link_libraries(TestResponse PRIVATE Let)

add_test(NAME LetTestResponse COMMAND TestResponse)

# --- network/server.c ---
add_executable(TestServer network/server.c)

target_compile_options(TestServer PRIVATE "-UNDEBUG")
target_link_libraries(TestServer PRIVATE Let)

add_test(NAME LetTestServer COMMAND TestServer)

# --- storage/crc.c ---
add_executable(TestCRC storage/crc.c)

target_compile_options(TestCRC PRIVATE "-UNDEBUG")
target_link_libraries(TestCRC PRIVATE Let)

add_test(NAME LetTestCRC COMMAND TestCRC)

# --- account.c ---
add_executable(TestAccount account.c)

target_compile_options(TestAccount PRIVATE "-UNDEBUG")
target_link_libraries(TestAccount PRIVATE Let)

add_test(NAME LetTestAccount COMMAND TestAccount)

# --- guard.c ---
add_executable(TestGuard guard.c)

target_compile_options(TestGuard PRIVATE "-UNDEBUG")
target_link_libraries(TestGuard PRIVATE Let)

add_test(NAME LetTestGuard COMMAND TestGuard)

# --- state.c ---
add_executable(TestState state.c)

target_compile_options(TestState PRIVATE "-UNDEBUG")
target_link_libraries(TestState PRIVATE Let)

add_test(NAME LetTestState COMMAND TestState)
