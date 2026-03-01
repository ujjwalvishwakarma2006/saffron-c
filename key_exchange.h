#ifndef KEY_EXCHANGE_H
#define KEY_EXCHANGE_H

// Server calls the following function to share his public key
void send_server_public_key();

// Client calls this function to receive server's public key
void recv_server_public_key();

// Client calls this function to generate a session for every new session
void generate_session_key();

// Client calls this function to encrypt the session key using server's public key
void encrypt_session_key();

// Client calls this function to generate a session key and share with the server
void send_session_key();

// Server calls this function to receive the session key from client
void recv_session_key();

// Server calls this function to decrypt the encrypted session key
void decrypt_session_key();

#endif // !KEY_EXCHANGE_H