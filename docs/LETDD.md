# LetDD

`LetDD` is a secure, Go-based API daemon built to expose an HTTP interface and forward ledger operations to `LetDB`.

---

## Security Architecture

- **Authentication:** If `LETDD_TOKEN` is configured, the authorization middleware rejects any request lacking the
  correct authorization header.
- **Permissions:** The `LETDD_PERMISSIONS` configuration strictly gates the execution of read or write operations based
  on defined flags.
- **Rate Limiting:** The rate-limiting middleware enforces the given configuration for transfer requests.

---

## REST Protocol

`LetDD` exposes a minimal, efficient set of endpoints. It translates requests into the compact TCP protocol required by
the internal storage engine.

### API Routes

| Method | Endpoint        | Description                           |
|--------|-----------------|---------------------------------------|
| POST   | `/accounts`     | Creates a new ledger account.         | 
| GET    | `/accounts/:id` | Retrieves current account state.      | 
| PUT    | `/accounts/:id` | Updates account flag attributes.      |
| POST   | `/transfers`    | Executes a transfer between accounts. |

---

## Source References

| Description       | File Path                                           |
|-------------------|-----------------------------------------------------|
| Protocol Encoding | [LetDD/tcp/request.go](../LetDD/tcp/request.go)     |
| Protocol Decoding | [LetDD/tcp/response.go](../LetDD/tcp/response.go)   |
| HTTP Request      | [LetDD/http/request.go](../LetDD/http/request.go)   |
| HTTP Response     | [LetDD/http/response.go](../LetDD/http/response.go) |
| HTTP Server       | [LetDD/http/server.go](../LetDD/http/server.go)     |
| TCP Client        | [LetDD/tcp/client.go](../LetDD/tcp/client.go)       |
| Command Line      | [LetDD/config/config.go](../LetDD/config/config.go) |
