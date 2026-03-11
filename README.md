<center>

```
███████╗ █████╗ ███████╗███████╗██████╗  ██████╗ ███╗   ██╗
██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██╔═══██╗████╗  ██║
███████╗███████║█████╗  █████╗  ██████╔╝██║   ██║██╔██╗ ██║
╚════██║██╔══██║██╔══╝  ██╔══╝  ██╔══██╗██║   ██║██║╚██╗██║
███████║██║  ██║██║     ██║     ██║  ██║╚██████╔╝██║ ╚████║
╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝     ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝
```
*Encrypted peer-to-peer chat over TCP*
</center>

> **Note**: This version of the project is no longer maintained actively. Maintainers of the project chose `Rust` to rewrite this project for the ease of development that can be found [here](https://github.com/ujjwalvishwakarma2006/saffron). Instead of just being a rewrite of the project, the `Rust` version also addresses most of the limitations mentioned in [Limitations and Vulnerabilities](#limitations-and-vulnerabilities) section and is improving constantly. If you do not see the Rust repository through the link, it means that the rewrite is in progress. Be Patient ( :

---

## Requirements

| Dependency | Purpose | Install |
|---|---|---|
| `ncursesw` | TUI rendering | `sudo apt-get install libncursesw5-dev` or `libncurses-dev`|
| `openssl` | All cryptographic operations | `sudo apt-get install openssl` |

Apart from these two, `gcc` and `make` are also required which are by default present in linux distros. 

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

## Limitations and Vulnerabilities

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

## Usage

```
Usage: ./saffron --server|--client [OPTIONS]

Options:
  --help            Show this help message
  --server          Run in server mode
  --client          Run in client mode
  -ip <address>     Server IP address (default: 127.0.0.1)
  -mp <port>        Message channel port (default: 8080)
  -fp <port>        File transfer port  (default: 8081)
```

### In-app commands

| Command | Action |
|---|---|
| `/f <filepath>` | Send a file to the peer |
| `/q` | Quit Saffron |
| Any other input | Send as a text message (Default) |

## Contribution

There are no formal contribution guidelines yet. At minimum, please provide a clear explanation of your changes — if you are fixing a bug, include at least one concrete failure case that reproduces it.

## License

This project is licensed under the [MIT License](LICENSE).