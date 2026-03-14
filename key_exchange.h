#ifndef KEY_EXCHANGE_H
#define KEY_EXCHANGE_H

/* ============================= KEY EXCHANGE PROTOCOL ============================= */
/*      Since the protocol is implemented on the application layer, these            */
/*      functions are called in the same order as they are written as soon as a      */
/*      TCP connection is established.                                               */
/* ================================================================================= */

/* Display and refresh after printing to the window */
void display_and_refresh(WINDOW* window, char* message);

/* Step 1a: Server sends its certificate to the client */
void server_send_certificate();

/* Step 1b: Client receives the certificate */
void client_recv_certificate();

/* Step 1c: Client verifies server's certificate against root CA's certificate */
void client_verify_certificate();

/* Step 2a: Client sends its certificate to the server */
void client_send_certificate();

/* Step 2b: Server receives the certificate */
void server_recv_certificate();

/* Step 2c: Server verifies client's certificate against root CA's certificate */
void server_verify_certificate();

/* Step 3a: Server generates Diffie-Hellman parameters */
void server_generate_dh_params();

/* Step 3b: Server generates its public key and appends in the dh_param file */
void server_generate_dh_pkey();

/* Step 3c: Server signs Diffie-Hellman packet with it's RSA private key */
void server_sign_dh_packet();

/* Step 3d: Server sends signed DH packet to the client */
void server_send_signed_dh();

/* Step 4: Client receives the signed DH packet */
void client_recv_signed_dh();

/* Step 5: Client extracts the contents of signed DH packet if the signature is valid */
void client_extract_dh_packet();

/* Step 5a: Client generates its public key based on received DH parameters */
void client_generate_dh_pkey();

/* Step 5b: Client signs its public key with its RSA private key */
void client_sign_dh_pkey();

/* Step 5c: Client sends its signed public key to the server */
void client_send_signed_dh_pkey();

/* Step 6: Server receives signed public key from the client */
void server_recv_signed_dh_pkey();

/* Step 7: Server extracts client's public key if it's signature is verified */
void server_extract_dh_pkey();

/* Step 8a: Client derives the secret key */
void client_derive_secret_key();

/* Step 8a: Server derives the secret key */
void server_derive_secret_key();

#endif