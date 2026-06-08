#include "let/cli.h"

#include <errno.h>
#include <getopt.h>
#include <stdio.h>

let_cli_t let_cli_empty(void) {
    return (let_cli_t){
        .storage_file = (char *) LET_CLI_DEFAULT_STORAGE_FILE,

        .port = LET_CLI_DEFAULT_PORT,
        .backlog = LET_CLI_DEFAULT_BACKLOG,

        .help = false,
        .version = false,
    };
}

let_error_t let_cli_parse(let_cli_t *cli,
                          const int argc,
                          char **argv) {
    let_error_t error = let_error_none();

    const struct option long_options[6] = {
        {"help", no_argument, nullptr, 'h'},
        {"version", no_argument, nullptr, 'v'},
        {"port", required_argument, nullptr, 'p'},
        {"backlog", required_argument, nullptr, 'b'},
        {"file", required_argument, nullptr, 'f'},
        {nullptr, 0, nullptr, 0}
    };

    char *end;
    int option;

    while ((option = getopt_long(argc, argv, "hvp:b:f:", long_options, nullptr)) != -1) {
        switch (option) {
            case 'h': {
                cli->help = true;
                break;
            }
            case 'v': {
                cli->version = true;
                break;
            }
            case 'p': {
                const auto value = strtoul(optarg, &end, 10);
                if (end == optarg || errno == ERANGE || value > LET_U16_MAX) {
                    error = let_error_new(LET_ERROR_ID_CLI, LET_ERROR_CLI_INVALID_PORT);
                    break;
                }

                cli->port = (let_u16_t) value;
                break;
            }
            case 'b': {
                const auto value = strtoul(optarg, &end, 10);
                if (end == optarg || errno == ERANGE || value > LET_U16_MAX) {
                    error = let_error_new(LET_ERROR_ID_CLI, LET_ERROR_CLI_INVALID_BACKLOG);
                    break;
                }

                cli->backlog = (let_u16_t) value;
                break;
            }
            case 'f': {
                cli->storage_file = optarg;
                break;
            }
            default: {
                error = let_error_new(LET_ERROR_ID_CLI, LET_ERROR_CLI_INVALID_OPTION);
                break;
            }
        }
    }

    return error;
}

void let_cli_help(void) {
    printf("Usage: LetDB [OPTIONS]\n"
        "\n"
        "Options:\n"
        "-h, --help              Show this help message and exit\n"
        "-v, --version           Show program version and exit\n"
        "-p, --port <PORT>       Specify the port number to listen on\n"
        "-b, --backlog <SIZE>    Set the connection backlog size\n"
        "-f, --file <PATH>       Specify the file to use\n"
        "\n"
        "Examples:\n"
        "LetDB --port 8080 -b 4\n"
        "LetDB -f instance.db\n");
}

void let_cli_version(void) {
    printf("LetDB v0.0.0\n");
}
