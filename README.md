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

- **End-to-end encrypted** using AES-256-CBC for messages and files, with an Authenticated Diffie-Hellman Key Exchange (via OpenSSL) ensuring forward secrecy.
- **Mutual Certificate-based authentication:** Both the server and the client exchange their X.509 certificates and verify them against a trusted Root CA using OpenSSL CMS before proceeding.
- **Non-repudiation and Integrity:** Every single message and file transmitted is encrypted and then digitally signed using modern OpenSSL CMS syntax, guaranteeing authenticity and preventing tampering.
- **Dynamic Session Keys:** A shared symmetric AES-256 key is derived dynamically on both ends using the exchanged Diffie-Hellman parameters.
- **Dual-channel transfer:** Separate TCP connections (separate ports) for messages and files running concurrently.
- **Threaded architecture:** Two dedicated threads handle incoming messages and files; the main thread handles all outgoing transfers.
- **TUI built with ncurses** — Header with ASCII logo and connection info, scrollable color-coded log window, dedicated input window, and a hint bar.
- **Color-coded log:** `You:` in green, peer name in magenta, file transfer logs in yellow, filenames in cyan.
- **Dynamic input buffer** that grows as needed via `realloc()`.

---

## Limitations and Vulnerabilities

| Severity | Issue |
|---|---|
| Security | Filenames and their lengths are currently transmitted unencrypted before the file contents are sent. |
| Security | Vulnerable to DoS attacks — `MSG_WAITALL` blocks indefinitely if a peer connects but sends incomplete or no data, hanging the entire session with no timeout. |
| Feature | Only one client can connect at a time — no support for multiple simultaneous connections. |
| Feature | No terminal resize handling — TUI layout is fixed at startup dimensions, and resizing can break the layout. |
| Dependency | All cryptographic operations are performed by forking `openssl` as child processes, requiring the `openssl` binary to be installed and available on the system `PATH`. |
| Performance | Creating child fork processes for every cryptographic operation overheads the CPU. Integrating `libcrypto` or `libssl` programmatically would drastically improve performance. |
| Performance | The input buffer starts at a single byte and grows one character at a time via `realloc()`, causing a heap allocation on every keystroke. Starting with a reasonably-sized fixed capacity (e.g., 256 bytes) and only reallocating when input exceeds it would eliminate most of these redundant allocations. |

---

## Setup

### 1. Clone and setup server side 
```bash
git clone https://github.com/ujjwalvishwakarma2006/saffron-c
cd saffron-c
mkdir -p .saffron-certs
```


### 2. Generate server credentials (run once on the server machine)

Here, in our case, the Root CA is the same as the server. Therefore, we use a single command to generate a certificate along with a private key. This is for simulation purposes only. We assume that this procedure has already happened via some secure means. 

```bash

cd .saffron-certs 

# On the Server side: Generate Root CA key and self-signed certificate
openssl req -new -x509 -sha256 -keyout server_rsa_skey.pem -out server_certificate.pem -days 365 -nodes -newkey rsa:2048

# Copy the server's certificate to root ca's certificate
cp server_certificate.pem rootca_certificate.pem

# Generate private key and Certificate Signing Request (CSR) for the client
openssl req -new -newkey rsa:2048 -nodes \  
  -keyout client_rsa_skey.pem \
  -out client_request.csr \
  -subj "/C=IN/ST=RJ/L=Jaipur/O=Saffron/CN=Client"

# Sign the client's request using Root CA's secret(private) key
openssl x509 -req -in client_request.csr \
  -CA rootca_certificate.pem \
  -CAkey server_rsa_skey.pem \
  -CAcreateserial \
  -out client_certificate.pem \
  -days 365 \
  -sha256

# Check files
ls
client_certificate.pem
client_request.csr
client_rsa_skey.pem
rootca_certificate.pem
server_certificate.pem
server_rsa_skey.pem

# Remove client CSR
rm client_request.csr

# Move client's certificate, key and the root ca's certificate to the .saffron-certs folder in the folder where you are testing from client side. 
# For example, the server side was
~/saffron-c

mkdir -p ~/client-saffron/.saffron-certs
cd client-saffron/.saffron-certs
cp ../../saffron-c/rootca_certificate.pem
mv ../../saffron-c/{client_certificate.pem,client_rsa_skey.pem}

# The final state of ~/saffron-c/.saffron-certs folder
ls
rootca_certificate.pem
server_certificate.pem
server_rsa_skey.pem

# The final state of ~/client-saffron/.saffron-certs folder
ls
rootca_certificate.pem
client_certificate.pem
client_rsa_skey.pem
```

> This certificate should ideally be shared with the client in some secure manner beforehand as one of the root CA certificates (`rootca_certificate.pem`).

### 3. Compile

```bash
make
```

### 4. Run

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

## Encryption Protocol

The protocol performs mutual verification and utilizes Diffie-Hellman Key Exchange combined with Digital Signatures for Forward Secrecy, Integrity, and Non-repudiation:

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

1. Server sends its X.509 certificate to the client, and the client sends its X.509 certificate to the server. Both ends verify certificates against the Root CA certificate.
2. Server generates Diffie-Hellman parameters, generates its DH key pair, merges the parameters and the public key into one payload, signs the payload using its RSA key matrix via `openssl cms`, and sends it to the client.
3. The client receives the payload, verifies the CMS signature using the server's cert, extracts the DH parameters and Server DH public key.
4. The client subsequently establishes its own DH key pair mapped to the received params, signs its public key using its own RSA key, and sends it to the server where it's verified.
5. Both sides compute the Shared Diffie-Hellman secret independently using `openssl pkeyutl -derive`.
6. Subsequent messages and file chunks are encrypted using the derived shared secret via `openssl enc -aes-256-cbc`, and the resulting ciphertexts are CMS-signed before being sent, requiring verification and extraction upon reading on the other end.

---

## Contribution

There are no formal contribution guidelines yet. At minimum, please provide a clear explanation of your changes — if you are fixing a bug, include at least one concrete failure case that reproduces it.

## License

This project is licensed under the [MIT License](LICENSE).
