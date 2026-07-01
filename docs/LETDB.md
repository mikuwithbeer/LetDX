# LetDB

`LetDB` is a robust, C-based storage engine built for the durable persistence of ledger operations.

---

## WAL Architecture

### On-Disk Format

- **Header:** An 8-byte magic string (`LET__WAL`) followed by a 2-byte version identifier.
- **Entries:** Each record contains a standard header (`let_storage_wal_entry_header_t`) and a type-specific payload (
  `let_storage_wal_entry_add_account_t`, `let_storage_wal_entry_make_transfer_t`, etc.).
- **Checksum:** Entries are written as a `let_storage_wal_entry_safe_t`, which appends a 4-byte **Castagnoli CRC32**
  checksum to the entry bytes for data verification.

### Replay Behavior

- **Validation:** Upon initialization, `LetDB` validates the header and replays all entries
  sequentially verifying the checksum of every record.
- **Failure:** If a checksum mismatch is detected, the engine might truncate the WAL at the last valid entry and
  continue replaying from there or report and abort the entire process depending on the strategy.
- **Durability:** During writes, the engine writes the entry and its checksum, then executes a system sync to guarantee
  physical disk persistence before acknowledging the operation to the client.

---

## TCP Protocol

`LetDB` implements a lightweight, line-oriented ASCII protocol. Each request is a single string terminated by a
newline (`\n`), with the initial character acting as the command token.

### Client Requests

| Command        | Syntax                           | Description                                          |
|----------------|----------------------------------|------------------------------------------------------|
| Magic          | `;\n`                            | Server replies with `LET\n`.                         |
| Add Account    | `+wal_id credits debits flags\n` | Creates a new account.                               |
| Make Transfer  | `%wal_id from_id to_id amount\n` | Executes a transfer between accounts.                |
| Get Account    | `?account_id\n`                  | Retrieves current account details.                   |
| Database Count | `#\n`                            | Requests total counts for accounts and transactions. |
| Update Account | `=wal_id account_id flags\n`     | Modifies account flag attributes.                    |
| Close          | `.\n`                            | Terminates the TCP connection.                       |

### Server Responses

| Response            | Syntax                                   | Description                                                |
|---------------------|------------------------------------------|------------------------------------------------------------|
| Magic               | `LET\n`                                  | Successful acknowledgment.                                 |
| Account ID          | `AID hex_id\n`                           | Success payload containing the newly generated account ID. |
| Account Information | `ACC credits_hex debits_hex flags_hex\n` | Current state of the requested account.                    |
| Database Count      | `LEN accounts_hex transactions_hex\n`    | Total recorded accounts and transactions.                  |
| Success             | `OKE\n`                                  | Generic success acknowledgment (no additional payload).    |
| Error               | `ERR code_hex\n`                         | Operation failure, accompanied by a numeric error code.    |

### Encoding Guidelines

1. Request parameters are treated as ASCII decimal-like tokens, but the internal parser supports optional underscores
   for readability.
2. Response integers are encoded as hex pairs representing raw big-endian byte sequences.
3. The server strictly tracks transaction sequence numbers. Clients must provide the requested id, which the server
   validates and persists.

---

## Interact with Server

You can use `nc` (netcat) to interact with the `LetDB` server:

```text
client: #
server: LEN 00 00
client: +0 0 1000 3
server: AID 00
client: +0 255 255 3
server: ERR 1392
client: +1 255 255 3
server: AID 01
client: #
server: LEN 02 02
client: %2 0 1 67
server: OKE
client: ?0
server: ACC 43 03E8 03
client: ?1
server: ACC FF 0142 03
```

---

## Source References

| Description        | File Path                                                                       |
|--------------------|---------------------------------------------------------------------------------|
| WAL Definitions    | [LetDB/include/let/storage/wal.h](../LetDB/include/let/storage/wal.h)           |
| WAL Implementation | [LetDB/src/storage/wal.c](../LetDB/src/storage/wal.c)                           |
| Request Decoding   | [LetDB/src/network/request/decoder.c](../LetDB/src/network/request/decoder.c)   |
| Response Encoding  | [LetDB/src/network/response/encoder.c](../LetDB/src/network/response/encoder.c) |
| TCP Server         | [LetDB/src/network/server.c](../LetDB/src/network/server.c)                     |
| Error Handling     | [LetDB/src/error.c](../LetDB/src/error.c)                                       |
| Command Line       | [LetDB/src/cli.c](../LetDB/src/cli.c)                                           |
