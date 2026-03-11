#ifndef KEY_EXCHANGE_H
#define KEY_EXCHANGE_H

/* ============================= KEY EXCHANGE PROTOCOL ============================= */
/*      Since the protocol is implemented on the application layer, these            */
/*      functions are called in the same order as they are written once as soon      */
/*      as TCP connection is established.                                            */
/* ================================================================================= */

/* Step 1a: Server sends its certificate to the client */
void server_send_certificate();

/* Step 1b: Client receives the certificate */
void client_recv_certificate();

/* Step 2: Client verifies the certificate via root CA's certificate */
void client_verify_certificate();

/* Step 3: Client generates a session key for further communication */
void client_generate_session_key();

/* Step 4: Client encrypts the session key using server's certificate */
void client_encrypt_session_key();

/* Step 5a: Client sends the encrypted session key to the server */
void client_send_session_key();

/* Step 5b: Server receives the encrypted session key */
void server_recv_session_key();

/* Step 6: Server decrypts the encrypted session key using its private key */
void server_decrypt_session_key();

/* Further communications occur encrypted via the shared secret key */

#endif