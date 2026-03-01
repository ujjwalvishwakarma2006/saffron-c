#ifndef KEY_EXCHANGE_H
#define KEY_EXCHANGE_H

// Server calls the following function to share his public key
void server_send_certificate();

// Client calls this function to receive server's public key
void client_recv_certificate();

// Verify certificate of the server
void client_verify_certificate();

// Client calls this function to generate a session for every new session
void client_generate_session_key();

// Client calls this function to encrypt the session key using server's public key
void client_encrypt_session_key();

// Client calls this function to generate a session key and share with the server
void client_send_session_key();

// Server calls this function to receive the session key from client
void server_recv_session_key();

// Server calls this function to decrypt the encrypted session key
void server_decrypt_session_key();

#endif // !KEY_EXCHANGE_H