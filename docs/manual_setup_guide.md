# Saffron Manual Setup Guide

Welcome to the manual setup guide for Saffron. If you are reading this, you either don't trust the automated setup script, or you just really enjoy typing OpenSSL commands. Either way, you are in the right place. Grab a beverage, stretch your fingers, and let's get into it.

## Step 1: The Land Grab (Creating Directories)
First, we need to create the folders for the client and server. They need their own space so they don't fight over their highly sensitive certificates.

```bash
mkdir -p server/.saffron-certs
mkdir -p client/.saffron-certs
```

## Step 2: Forging the Server's Identity
The server needs a Root CA key and a self-signed certificate. Think of it as the server printing its own ID card and validating it.

```bash
cd server/.saffron-certs
openssl req -new -x509 -sha256 -keyout server_rsa_skey.pem -out server_certificate.pem -days 365 -nodes -newkey rsa:2048 -subj "/C=IN/ST=RJ/L=Jaipur/O=Saffron/CN=Server CA"

# Let's also copy the server certificate to be our Root CA
cp server_certificate.pem rootca_certificate.pem
```

## Step 3: The Client's Application
The client needs an ID too, but it has to ask the server (our Root CA) to sign it. First, the client creates a private key and a Certificate Signing Request (CSR).

```bash
# Generate the client's key and CSR
openssl req -new -newkey rsa:2048 -nodes -keyout client_rsa_skey.pem -out client_request.csr -subj "/C=IN/ST=RJ/L=Jaipur/O=Saffron/CN=Client"

# The server (pretending to be the bouncing bouncer) signs it
openssl x509 -req -in client_request.csr -CA rootca_certificate.pem -CAkey server_rsa_skey.pem -CAcreateserial -out client_certificate.pem -days 365 -sha256

# We clean up the evidence (the CSR is no longer needed)
rm client_request.csr
```

## Step 4: Diffie-Hellman Parameter Generation
This part is the mathematical heavy-lifting. It generates the Diffie-Hellman parameters that both sides need to establish a shared secret. It might take a few seconds, so patience is key here.

```bash
openssl genpkey -genparam -algorithm DH -out dhp_file.pem
```

## Step 5: Distributing the Loot
Now that everything is generated in the server's directory, we need to give the client its rightful belongings. 

```bash
# Move the client's certificate and key to its directory
mv client_certificate.pem ../../client/.saffron-certs/
mv client_rsa_skey.pem ../../client/.saffron-certs/

# Give the client a copy of the Root CA and the DH parameters
cp rootca_certificate.pem ../../client/.saffron-certs/
cp dhp_file.pem ../../client/.saffron-certs/

# Go back to the main project directory
cd ../../
```

## Step 6: Compiling and Placing Binaries
Time to build the actual code. Make sure you are in the root directory where the `Makefile` is.

```bash
make

# Give both the client and server their very own executable 
cp saffron client/
cp saffron server/
```

## Step 7: Victory
You made it! You can now run the server and client in two separate terminal windows.

Window 1 (Server):
```bash
cd server
./saffron --server
```

Window 2 (Client):
```bash
cd client
./saffron --client
```

See? That was not so bad. But next time, maybe just run `./setup.sh`.