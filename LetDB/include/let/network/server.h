#ifndef LET_NETWORK_SERVER_H
#define LET_NETWORK_SERVER_H

/**
 * @file server.h
 * @brief The network server interface.
 *
 * This interface provides the necessary functions to create a TCP server that can
 * accept incoming connections, read requests, and send responses.
 */

#include "let/network/request/decoder.h"
#include "let/network/response/encoder.h"

#include <netinet/in.h>

// -----------------------------------------------------------------------------
// Compile-Time Constants
// -----------------------------------------------------------------------------

// The maximum length of the network buffer for both read and write.
constexpr let_u8_t LET_NETWORK_BUFFER_LENGTH = 128;

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

/**
 * @brief The network server structure.
 *
 * @note This structure is used for both the server and client connections.
 * The server listens for incoming connections, while the client represents an accepted connection.
 */
typedef struct {
    struct sockaddr_in address;
    let_i32_t handle;
    let_u16_t port;

    let_u8_t read_buffer[LET_NETWORK_BUFFER_LENGTH];
    let_u8_t write_buffer[LET_NETWORK_BUFFER_LENGTH];

    let_size_t read_buffer_index;
    let_size_t write_buffer_index;
} let_network_server_t;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Returns an empty, zero-initialized network server struct.
 */
[[nodiscard]] let_network_server_t let_network_server_empty(void);

/**
 * @brief Initializes the network server.
 */
let_error_t let_network_server_init(let_network_server_t *network_server,
                                    let_u16_t port,
                                    let_u16_t backlog);

/**
 * @brief Accepts an incoming connection and initializes the network client.
 */
let_error_t let_network_server_accept(const let_network_server_t *network_server,
                                      let_u32_t read_timeout,
                                      let_u32_t write_timeout,
                                      let_network_server_t *network_client);

/**
 * @brief Reads and decodes a request structure from a client.
 *
 * @note Blocks depending on `read_timeout` thresholds until the entire request is received.
 */
let_error_t let_network_client_read(let_network_server_t *network_client,
                                    let_network_request_t *request);

/**
 * @brief Encodes and writes a response structure to a client.
 *
 * @note Blocks depending on `write_timeout` thresholds until the entire response is sent.
 */
let_error_t let_network_client_write(let_network_server_t *network_client,
                                     let_network_response_t response);

/**
 * @brief Closes the network server or client, releasing any associated resources.
 */
void let_network_close(let_network_server_t *network_server);

#endif //LET_NETWORK_SERVER_H
