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

/* Step 1c: Client verifies server's certificate via root CA's certificate */
void client_verify_certificate();

/* Step 2a: Client sends its certificate to the client */
void client_send_certificate();

/* Step 2b: Server receives the certificate */
void server_recv_certificate();

/* Step 2c: Server verifies client's certificate via root CA's certificate */
void server_verify_certificate();

/* Step 3a: Server generates Diffie-Hellman parameters */
void server_generate_dh_params();

/* Step 3b: Server generates its public key and appends in the dh_param file */
void server_generate_public_key();

/* Step 3c: Server signs Diffie-Hellman packet with it's RSA private key */
void server_sign_dh_packet();

/* Step 3d: Server sends signed DH packet to the client */
void server_send_signed_dh();

/* Step 4: Client verfies DH packet's signature using server's RSA public key */
void client_vrfy_dh_sign();

/* Step 5: Client sends its public key to the server */
void client_send_public_key();

/* Step 6: Both Client and server derive the secret key */
void derive_secret_key();

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