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

> **Note**: This C version of the project is no longer actively maintained. The maintainers are rewriting the project in [Rust](https://github.com/ujjwalvishwakarma2006/saffron) for a better developer experience. Beyond a simple rewrite, the Rust version resolves most of the issues detailed in the [Limitations and Vulnerabilities](#limitations-and-vulnerabilities) section and is continuously improving. *(If the repository link is unavailable, the rewrite is still in progress. Thank you for your patience!)* ( :

---

## Requirements

| Dependency | Purpose | Install |
|---|---|---|
| `ncursesw` | TUI rendering | `sudo apt-get install libncursesw5-dev` or `libncurses-dev`|
| `openssl` | All cryptographic operations | `sudo apt-get install openssl` |

---

## Features
|||
|---|---|
| **End-to-end encrypted** | Uses AES-256-CBC for messages and files, with an Authenticated Diffie-Hellman Key Exchange (via OpenSSL) ensuring forward secrecy. | |
| **Mutual Certificate-based authentication** | Both the server and the client exchange their X.509 certificates and verify them against the trusted Root CA using OpenSSL CMS before proceeding. |
| **Non-repudiation and Integrity** | Every single message and file transmitted is encrypted and then digitally signed using modern OpenSSL CMS syntax, guaranteeing authenticity and preventing tampering. |
| **Dynamic Session Keys** | A shared symmetric AES-256 key is derived dynamically on both ends using the exchanged Diffie-Hellman parameters. |
| **Dual-channel transfer** | Separate TCP connections (separate ports) for messages and files running concurrently. |
| **Threaded architecture** | Two dedicated threads handle incoming messages and files; the main thread handles all outgoing transfers. |
| **TUI built with ncurses** | Header with ASCII logo and connection info, scrollable color-coded log window, dedicated input window, and a hint bar. |

---

## Limitations and Vulnerabilities

| Severity | Issue |
|---|---|
| **Security** | Filenames and their lengths are currently transmitted unencrypted before the file contents are sent. |
| **Security** | Vulnerable to DoS attacks — `MSG_WAITALL` blocks indefinitely if a peer connects but sends incomplete or no data, hanging the entire session with no timeout. |
| **Feature** | Only one client can connect at a time — no support for multiple simultaneous connections. |
| **Feature** | No terminal resize handling — TUI layout is fixed at startup dimensions, and resizing can break the layout. |
| **Feature** | One of the peers should be always online to accept connections. And that too can only be the server (by using `--server` flag) as of now. This doesn't make any sense. |
| **Dependency** | All cryptographic operations are performed by forking `openssl` as child processes, requiring the `openssl` binary to be installed and available on the system `PATH`. |
| **Performance** | Creating child fork processes for every cryptographic operation overheads the CPU. Integrating `libcrypto` or `libssl` programmatically would drastically improve performance. |
| **Performance** | The input buffer starts at a single byte and grows one character at a time via `realloc()`, causing a heap allocation on every keystroke. Starting with a reasonably-sized fixed capacity (e.g., 256 bytes) and only reallocating when input exceeds it would eliminate most of these redundant allocations. |

---

## Setup

### Assumptions
- Both the client and the server are securely provided with the Root CA's certificate, and both have their respective certificates signed by this same Root CA. The provided setup script simulates this environment by automatically generating the necessary certificates for both parties.

1. Clone the repository. 
2. Run the [Setup Script](./setup.sh) or follow the [Manual Setup Guide](./docs/manual_setup_guide.md) for detailed information on each setup step.

To start again from zero, clean everything by running:
```bash
rm -rf server/ client/ objects/ saffron
```

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

---

## Key Exchange Protocol

The protocol performs mutual verification and utilizes **Diffie-Hellman Key Exchange** for **forward secracy** combined with Digital Signatures for **Integrity**, **Authentication** and **Non-repudiation** at once:

```
Client                                                  Server
  |                                                       |
  |  <----- server_certificate.pem -----------------------|  (1)
  |                                                       |
  |  ------ client_certificate.pem ---------------------->|  (2)
  |                                                       |
  | verify server cert                      verify client cert  
  |                                                       |
  |                              generate DH params & key pair
  |                             sign (DH param + Server DH PKEY)
  |                                                       |
  |  <----- signed (DH param + Server DH PKEY) -----------|  (3)
  |                                                       |
  | verify signature, split & extract DH fields           |
  | generate Client DH key pair                           |
  | sign Client DH PKEY                                   |
  |                                                       |
  |  ------ signed (Client DH PKEY) --------------------->|  (4)
  |                                                       |
  |                                verify signature & extract
  |                                                       |
  |           [Both parties derive DH Secret Key]         |
  |                                                       |
  |  <======== AES-256-CBC Encrypted & CMS Signed =======>|  (Subsequent comms over both channels)
```

1. Server sends its X.509 certificate to the client, and the client sends its X.509 certificate to the server. Both ends verify certificates against the Root CA certificate. From now onwards, everything goes to the other side digitally signed.
2. Server generates Diffie-Hellman parameters, generates its DH key pair, merges the parameters and the public key into one payload, signs the payload using its RSA key matrix via `openssl cms`, and sends it to the client.
3. The client receives the payload, verifies the CMS signature using the server's cert, extracts the DH parameters and Server DH public key.
4. The client subsequently establishes its own DH key pair mapped to the received params, signs its public key using its own RSA key, and sends it to the server where it's verified.
5. Both sides compute the Shared Diffie-Hellman secret independently using `openssl pkeyutl -derive`.
6. Subsequent messages and file chunks are encrypted using the derived shared secret via `openssl enc -aes-256-cbc`, and the resulting ciphertexts are CMS-signed before being sent, requiring verification and extraction upon reading on the other end.

---

## Contribution

There are no formal contribution guidelines yet. At minimum, please provide a clear explanation of your changes — if you are fixing a bug, include at least one concrete failure case that reproduces it.

---

## License

This project is licensed under the [MIT License](LICENSE).