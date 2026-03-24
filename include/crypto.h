#ifndef CRYPTO_H
#define CRYPTO_H

/* Redirects OpenSSL child-process stdout/stderr to /dev/null. */
void disable_terminal_output();

/* Waits for a child process and prints a formatted OpenSSL failure message. */
bool crypto_wait_and_check(int child_pid, const char* error_prefix);

/* Fork/exec wrapper used by crypto helpers to run one OpenSSL command. */
bool execute_crypto_command(char* command_args[], const char* error_prefix, bool suppress_output);

/* Verifies peer_cert against root_cert using OpenSSL certificate validation. */
bool verify_certificate(char* root_cert, char* peer_cert);

/* Generates Diffie-Hellman parameters and writes them to dh_params_file. */
bool generate_dh_params(char* dh_params_file);

/* Generates a DH private/public key pair from an existing DH parameter file. */
bool generate_dh_key_pair(
    char* dh_params_file,
    char* secret_key_file,
    char* public_key_file
);

/* Derives shared DH secret from local private key and peer public key. */
bool derive_dh_skey(
    char* local_dh_secret_key_file,
    char* peer_dh_public_key_file,
    char* derived_secret_file
);

/* CMS-signs in_file with signer credentials and writes signed payload to out_file. */
bool cms_sign_file(char* in_file, char* signer_cert, char* signer_skey, char* out_file);

/* Verifies CMS signature in in_file and extracts verified payload to out_file. */
bool cms_extract_file(char* in_file, char* root_cert, char* out_file);

/* Encrypts file_name into enc_file_name with AES-256-CBC using key_file. */
bool encrypt_file(char* file_name, char* enc_file_name, char* key_file);

/* Decrypts enc_file_name into file_name with AES-256-CBC using key_file. */
bool decrypt_file(char* enc_file_name, char* file_name, char* key_file);

#endif // !CRYPTO_H