#ifndef CRYPTO_H
#define CRYPTO_H

// Verify peer_cert against root_cert
void verify_certificate(char* root_cert, char* peer_cert);

// Generate Diffie-Hellman parameters in out_file
void generate_dh_params(char* dhp_file);

// Generate Diffie-Hellman pub-private key from DH parameters
void generate_dh_key_pair(char* dhp_file, char* skey_file, char* pkey_file);

// Only AES-256-CBC Mode
void encrypt_file(char* file_name, char* enc_file_name, char* key_file);

// Only AES-256-CBC Mode
void decrypt_file(char* enc_file_name, char* file_name, char* key_file);

// Modern sign and concatenate the signature with the file into a single file 
void cms_sign_file(char* in_file, char* signer_cert, char* signer_skey, char* out_file);

// Verify the signature and extract the file after verification in one single step
void cms_extract_file(char* in_file, char* root_cert, char* out_file);

#endif // !CRYPT_H