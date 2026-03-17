#ifndef CRYPTO_H
#define CRYPTO_H

// Utility function to disable outputting on terminal
void disable_terminal_output();

// Helper function to handle waitpid and error checking
bool crypto_wait_and_check(int child_pid, const char* error_prefix);

// Verify peer_cert against root_cert
bool verify_certificate(char* root_cert, char* peer_cert);

// Generate Diffie-Hellman parameters in out_file
bool generate_dh_params(char* dhp_file);

// Generate Diffie-Hellman public-private key-pair from DH parameters
bool generate_dh_key_pair(char* dhp_file, char* skey_file, char* pkey_file);

// Derive DH secret key, given both client and server public keys
bool derive_dh_skey(char* host_dh_skey, char* peer_dh_pkey, char* dh_skey_file);

// Modern sign and concatenate the signature with the file into a single file 
bool cms_sign_file(char* in_file, char* signer_cert, char* signer_skey, char* out_file);

// Verify the signature and extract the file after verification in one single step
bool cms_extract_file(char* in_file, char* root_cert, char* out_file);

// Only AES-256-CBC Mode
bool encrypt_file(char* file_name, char* enc_file_name, char* key_file);

// Only AES-256-CBC Mode
bool decrypt_file(char* enc_file_name, char* file_name, char* key_file);

#endif // !CRYPT_H