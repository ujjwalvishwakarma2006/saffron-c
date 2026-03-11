#ifndef CRYPTO_H
#define CRYPTO_H

// Only AES-256-CBC Mode
void encrypt_file(char* file_name, char* enc_file_name, char* key_file);

// Only AES-256-CBC Mode
void decrypt_file(char* enc_file_name, char* file_name, char* key_file);

// HMAC
void generate_hmac(char* file_name, char* )

#endif // !CRYPT_H