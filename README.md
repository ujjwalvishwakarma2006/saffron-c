<div align="center">

```
███████╗ █████╗ ███████╗███████╗██████╗  ██████╗ ███╗   ██╗
██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██╔═══██╗████╗  ██║
███████╗███████║█████╗  █████╗  ██████╔╝██║   ██║██╔██╗ ██║
╚════██║██╔══██║██╔══╝  ██╔══╝  ██╔══██╗██║   ██║██║╚██╗██║
███████║██║  ██║██║     ██║     ██║  ██║╚██████╔╝██║ ╚████║
╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝     ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝
```

*Encrypted peer-to-peer chat over TCP*

</div>

---

## Requirements

| Dependency | Purpose | Install |
|---|---|---|
| `ncursesw` | TUI rendering | `sudo apt-get install libncursesw5-dev` |
| `openssl` | All cryptographic operations | `sudo apt-get install openssl` |
| `gcc` | Compilation | `sudo apt-get install build-essential` |
| `make` | Build system | included with `build-essential` |

> **WSL users:** Saffron runs on WSL. Use Windows Terminal for best ncurses compatibility. Run `ip -4 addr show` to find your WSL IP and pass it via `-ip`.

---

## Features

- **End-to-end encrypted** using AES-256-CBC for messages and files, and RSA-2048 + X.509 certificates for session key exchange.
- **Certificate-based authentication:** the server sends its X.509 certificate to the client, which verifies it against a trusted Root CA before proceeding.
- **Per-session symmetric key:** the client generates a fresh AES-256 key every session, encrypts it with the server's RSA public key extracted from the certificate, and sends it to the server.
- **Dual-channel transfer:** separate TCP connections (separate ports) for messages and files running concurrently.
- **Threaded architecture:** two dedicated threads handle incoming messages and files; the main thread handles all outgoing transfers.
- **TUI built with ncurses** — header with ASCII logo and connection info, scrollable color-coded log window, dedicated input window, and a hint bar.
- **Color-coded log:** `You:` in green, peer name in magenta, file transfer logs in yellow, filenames in cyan.
- **Dynamic input buffer** that grows as needed via `realloc()`.

---

## Encryption Protocol (Similar to TLS)

```
Client                                    Server
  |                                          |
  |  <---- server_certificate.pem -----------|  (over file socket)
  |                                          |
  | verify certificate against root CA       |
  |                                          |
  | generate AES-256 session key             |
  | encrypt session key with server pubkey   |
  |                                          |
  |  ----- encrypted session key ----------->|  (over file socket)
  |                                          |
  |         server decrypts session key      |
  |                                          |
  |  <===== AES-256-CBC encrypted session ==>|  (both channels)
```

1. Server sends its X.509 certificate (`server_cert_path`) to the client over the file socket.
2. Client verifies the certificate against the Root CA certificate (`root_ca_cert_path`) using `openssl verify`.
3. Client generates a 256-bit AES session key using `openssl rand`.
4. Client encrypts the session key with the server's public key (extracted from the certificate) using `openssl pkeyutl -certin`.
5. Client sends the encrypted session key to the server.
6. Server decrypts the session key using its RSA private key via `openssl pkeyutl`.
7. All subsequent messages and files are encrypted/decrypted using `openssl enc -aes-256-cbc` with the shared session key.

---

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

---

## Setup

### 1. Generate server credentials (run once on the server machine)

Here, in our case, the Root CA is same as the server. Therefore, we use a single command to generate everything in one go. 

```bash
# Generate Root CA key and self-signed certificate
openssl req -new -x509 -sha256 -keyout privkey.pem -out certificate.crt -days 365 -nodes -newkey rsa:2048
```

> This certificate should be share with the client in some secure manner beforehand as one of the root CA certificates (`root_ca_certificate.pem`).

### 2. Compile

```bash
make
```

### 3. Run

```bash
# On the server machine
./saffron --server -ip <server-ip>

# On the client machine
./saffron --client -ip <server-ip>
```

---

## Usage

```
Usage: ./saffron --server|--client [OPTIONS]

Options:
  --help            Show this help message
  --server          Run in server mode
  --client          Run in client mode
  -ip <address>     Server IP address (default: 10.0.2.4)
  -mp <port>        Message channel port (default: 8080)
  -fp <port>        File transfer port  (default: 8081)
```

### In-app commands

| Command | Action |
|---|---|
| `/f <filepath>` | Send a file to the peer |
| `/q` | Quit Saffron |
| Any other input | Send as a text message (Default) |

---

## Limitations and Vulnuerabilities

| Severity | Issue |
|---|---|
| 🔴 Security | Not forward secret — RSA key exchange means compromise of the server's private key exposes all past session keys. A Diffie-Hellman based exchange (e.g., ECDHE) would fix this. |
| 🔴 Security | Filenames and their lengths are transmitted unencrypted. |
| 🔴 Security | Vulnerable to DoS — `MSG_WAITALL` blocks indefinitely if a peer connects but sends incomplete or no data, hanging the entire session with no timeout. |
| 🔴 Security | No explicit integrity verification — there is no MAC or HMAC; correctness relies solely on AES-CBC padding, which cannot detect intentional tampering. An HMAC-SHA256 over ciphertext (encrypt-then-MAC) would fix this. |
| 🟡 Feature | Only one client can connect at a time — no support for multiple simultaneous connections. |
| 🟡 Feature | No terminal resize handling — TUI layout is fixed at startup dimensions. |
| 🔵 Dependency | All cryptographic operations are performed by forking `openssl` as child processes, requiring `openssl` to be installed and available on `PATH`. |
| 🔵 Performance | The input buffer starts at a single byte and grows one character at a time via `realloc()`, causing a heap allocation on every keystroke. Starting with a reasonably-sized fixed capacity (e.g., 256 bytes) and only reallocating when input exceeds it would eliminate most of these redundant allocations. |

A `Rust` rewrite of this project that addresses most of the above limitations is available [here](https://github.com/ujjwalvishwakarma2006/saffron).

## Contribution

There are no formal contribution guidelines yet. At minimum, please provide a clear explanation of your changes — if you are fixing a bug, include at least one concrete failure case that reproduces it.

## License

This project is licensed under the [MIT License](LICENSE).