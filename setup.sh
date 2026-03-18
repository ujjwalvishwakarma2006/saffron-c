#!/bin/bash

# ==========================================
# Configuration Variables
# ==========================================
SERVER_DIR="server"
CLIENT_DIR="client"
SERVER_CERTS_DIR="${SERVER_DIR}/.saffron-certs"
SERVER_TEMP_DIR="${SERVER_DIR}/.saffron-temp"
CLIENT_CERTS_DIR="${CLIENT_DIR}/.saffron-certs"

SERVER_CERT="server_certificate.pem"
SERVER_KEY="server_rsa_skey.pem"
ROOT_CA_CERT="rootca_certificate.pem"

CLIENT_CERT="client_certificate.pem"
CLIENT_KEY="client_rsa_skey.pem"
CLIENT_CSR="client_request.csr"

DH_PARAMS="dhp.pem"

# Exit immediately if a command exits with a non-zero status
set -e

# Error handling trap
handle_error() {
    echo "[!] Error occurred during setup on line $1."
    exit 1
}
trap 'handle_error $LINENO' ERR

echo "=== Saffron Setup Script ==="

# ==========================================
# Section 1: Directory Setup
# ==========================================
echo "[*] Creating directories for client, server, and certificates..."
mkdir -p "${SERVER_CERTS_DIR}" "${SERVER_TEMP_DIR}" "${CLIENT_CERTS_DIR}"
echo "[+] Directories created successfully."

# ==========================================
# Section 2: Server Certificate & Key Generation
# ==========================================
echo "[*] Generating Server Root CA key and self-signed certificate..."
cd "${SERVER_CERTS_DIR}"

# Provide a default subject so openssl doesn't prompt for input
openssl req -new -x509 -sha256 \
  -keyout "${SERVER_KEY}" \
  -out "${SERVER_CERT}" \
  -days 365 \
  -nodes -newkey rsa:2048 \
  -subj "/C=IN/ST=RJ/L=Jaipur/O=Saffron/CN=Server CA" > /dev/null 2>&1

cp "${SERVER_CERT}" "${ROOT_CA_CERT}"
echo "[+] Server certificates generated."

# ==========================================
# Section 3: Client Certificate & Key Generation
# ==========================================
echo "[*] Generating Client private key and Certificate Signing Request (CSR)..."
openssl req -new -newkey rsa:2048 -nodes \
  -keyout "${CLIENT_KEY}" \
  -out "${CLIENT_CSR}" \
  -subj "/C=IN/ST=RJ/L=Jaipur/O=Saffron/CN=Client" > /dev/null 2>&1

echo "[*] Signing the client's request using Root CA's private key..."
openssl x509 -req -in "${CLIENT_CSR}" \
  -CA "${ROOT_CA_CERT}" \
  -CAkey "${SERVER_KEY}" \
  -CAcreateserial \
  -out "${CLIENT_CERT}" \
  -days 365 \
  -sha256 > /dev/null 2>&1

rm "${CLIENT_CSR}"
echo "[+] Client certificates generated and signed."

# ==========================================
# Section 4: Diffie-Hellman Parameters Generation
# ==========================================
echo "[*] Generating Diffie-Hellman parameters (this might take a few seconds)..."
openssl genpkey -genparam -algorithm DH -out "${DH_PARAMS}" > /dev/null 2>&1
echo "[+] DH parameters generated."
mv "${DH_PARAMS}" "../../${SERVER_TEMP_DIR}/"

# ==========================================
# Section 5: Distributing Files
# ==========================================
echo "[*] Distributing certificates and keys to respective directories..."
mv "${CLIENT_CERT}" "../../${CLIENT_CERTS_DIR}/"
mv "${CLIENT_KEY}" "../../${CLIENT_CERTS_DIR}/"
cp "${ROOT_CA_CERT}" "../../${CLIENT_CERTS_DIR}/"
cd ../../
echo "[+] Files distributed."

# ==========================================
# Section 6: Verification
# ==========================================
echo "[*] Verifying generated files..."

check_file() {
    if [[ ! -f "$1" ]]; then
        echo "[!] Error: Missing required file: $1"
        exit 1
    fi
}

# Verify Server Files
check_file "${SERVER_CERTS_DIR}/${SERVER_CERT}"
check_file "${SERVER_CERTS_DIR}/${SERVER_KEY}"
check_file "${SERVER_CERTS_DIR}/${ROOT_CA_CERT}"
check_file "${SERVER_TEMP_DIR}/${DH_PARAMS}"

# Verify Client Files
check_file "${CLIENT_CERTS_DIR}/${CLIENT_CERT}"
check_file "${CLIENT_CERTS_DIR}/${CLIENT_KEY}"
check_file "${CLIENT_CERTS_DIR}/${ROOT_CA_CERT}"

echo "[+] Verification passed. All required files are present."

# ==========================================
# Section 7: Compilation
# ==========================================
echo "[*] Compiling the project..."
make > /dev/null
cp saffron "${CLIENT_DIR}/"
cp saffron "${SERVER_DIR}/"
echo "[+] Compilation successful."

echo ""
echo "=== Setup Completed Successfully ==="
echo "You can now run the server and client using the following commands:"
echo "-> Server: cd ${SERVER_DIR} && ./saffron --server"
echo "-> Client: cd ${CLIENT_DIR} && ./saffron --client"