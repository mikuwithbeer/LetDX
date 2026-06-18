# --- storage/crc.c ---
add_executable(TestCRC storage/crc.c)
target_link_libraries(TestCRC PRIVATE Let)
add_test(NAME LetTestCRC COMMAND TestCRC)

# --- account.c ---
add_executable(TestAccount account.c)
target_link_libraries(TestAccount PRIVATE Let)
add_test(NAME LetTestAccount COMMAND TestAccount)

# --- state.c ---
add_executable(TestState state.c)
target_link_libraries(TestState PRIVATE Let)
add_test(NAME LetTestState COMMAND TestState)
