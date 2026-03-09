## Code Structure

### `common.h / .c`
Global state shared across all modules: `app_mode`, sockets, ports, IP, buffers, all file paths, ncurses windows, and `fatal_error()`.

### `args.h / .c` - CLI argument parsing and usage output
- `void setup(int argc, char* argv[])` — parses `--server`/`--client`, `-ip`, `-mp`, `-fp`, `--help`.
- `void print_usage(char* program_name)` — prints usage to stderr.

### `tui.h / .c` - ncurses TUI setup, layout drawing, and input reading
- `void tui_init()` — initializes ncurses, sets up color pairs, draws the 4-section TUI layout (header, log, input, hint bar).
- `char* wgetstring(WINDOW* window)` — reads a dynamically-sized string from an ncurses window with backspace handling.

### `server.h / .c` - server-side socket creation, binding, and connection acceptance
- `int server_start(const char* ip, const int port, const char* label)` — creates, binds, and listens on a TCP socket. Returns listening fd.
- `int server_accept(const int listen_fd, const char* label)` — accepts the first incoming connection. Returns connected fd.

### `client.h / .c` - client-side TCP connection establishment
- `int client_connect(const char* ip, const int port, const char* label)` — establishes a TCP connection. Returns connected fd.

### `key_exchange.h / .c` - certificate exchange and RSA session key negotiation, both sides
- `void server_send_certificate()` — server sends its X.509 certificate to client.
- `void client_recv_certificate()` — client receives the certificate.
- `void client_verify_certificate()` — verifies certificate against Root CA via `openssl verify`. Calls `fatal_error()` on failure.
- `void client_generate_session_key()` — generates 32-byte AES key via `openssl rand`.
- `void client_encrypt_session_key()` — encrypts AES key with server's public key via `openssl pkeyutl -certin`.
- `void client_send_session_key()` — sends encrypted session key to server.
- `void server_recv_session_key()` — receives encrypted session key.
- `void server_decrypt_session_key()` — decrypts session key via `openssl pkeyutl`. Calls `fatal_error()` on failure.

### `crypto.h / .c` - symmetric AES-256-CBC file encryption and decryption
- `void crypto_encrypt(char* in_path, char* out_path, char* key_path)` — encrypts a file using AES-256-CBC (forks `openssl enc`).
- `void crypto_decrypt(char* in_path, char* out_path, char* key_path)` — decrypts a file using AES-256-CBC (forks `openssl enc -d`).

### `outgoing.h / .c` - main-thread input loop for sending outgoing messages and files
- `void msg_send(const int socket, char* message)` — writes message to temp file, encrypts it, delegates to `file_send()`.
- `void file_send(const int socket, char* filepath, const bool is_msg_file)` — encrypts and sends a file. Transmits filename, size, then data.
- `void outgoing_handle()` — main input loop. `/f <filepath>` sends a file, `/q` quits, anything else sends a message.

### `msg_recv.h / .c` - dedicated thread for receiving and displaying incoming messages
- `void* msg_recv()` — thread function. Continuously receives, decrypts, and displays incoming messages in the log window.

### `file_recv.h / .c` - dedicated thread for receiving, decrypting, and saving incoming files
- `void* file_recv()` — thread function. Continuously receives, decrypts, and saves incoming files. Displays colored transfer log.
