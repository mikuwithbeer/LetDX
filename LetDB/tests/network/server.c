#include "let/network/server.h"

#include <assert.h>

static let_network_server_t network_server;

static void test_network_server_empty(void) {
    network_server = let_network_server_empty();

    assert(network_server.handle == -1);
    assert(network_server.port == 0);

    assert(network_server.read_buffer_index == 0);
    assert(network_server.write_buffer_index == 0);
}

static void test_network_server_init(void) {
    assert(!let_error_exists(let_network_server_init(&network_server, 8080, 1)));
}

static void test_cleanup(void) {
    let_network_close(&network_server);
    assert(true);
}

int main(void) {
    test_network_server_empty();
    test_network_server_init();

    test_cleanup();
    return 0;
}
