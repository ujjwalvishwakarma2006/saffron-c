#ifndef KEY_EXCHANGE_H
#define KEY_EXCHANGE_H

/* ============================= KEY EXCHANGE PROTOCOL ============================= */
/*      Since the protocol is implemented on the application layer, these            */
/*      functions are called in the same order as they are written as soon as a      */
/*      TCP connection is established.                                               */
/* ================================================================================= */

/* Display and refresh after printing to the window */
bool display_and_refresh(WINDOW* window, char* message);

/* Step 1a: Server sends its certificate to the client */
bool server_send_certificate();

/* Step 1b: Client receives the certificate */
bool client_recv_certificate();

/* Step 1c: Client verifies server's certificate against root CA's certificate */
bool client_verify_certificate();

/* Step 2a: Client sends its certificate to the server */
bool client_send_certificate();

/* Step 2b: Server receives the certificate */
bool server_recv_certificate();

/* Step 2c: Server verifies client's certificate against root CA's certificate */
bool server_verify_certificate();

/* Step 3a: Server generates Diffie-Hellman parameters */
bool server_generate_dh_params();

/* Step 3b: Server generates its public key and appends in the dh_param file */
bool server_generate_dh_pkey();

/* Step 3c: Server signs Diffie-Hellman packet with it's RSA private key */
bool server_sign_dh_packet();

/* Step 3d: Server sends signed DH packet to the client */
bool server_send_signed_dh();

/* Step 4: Client receives the signed DH packet */
bool client_recv_signed_dh();

/* Step 5: Client extracts the contents of signed DH packet if the signature is valid */
bool client_extract_dh_packet();

/* Step 5a: Client generates its public key based on received DH parameters */
bool client_generate_dh_pkey();

/* Step 5b: Client signs its public key with its RSA private key */
bool client_sign_dh_pkey();

/* Step 5c: Client sends its signed public key to the server */
bool client_send_signed_dh_pkey();

/* Step 6: Server receives signed public key from the client */
bool server_recv_signed_dh_pkey();

/* Step 7: Server extracts client's public key if it's signature is verified */
bool server_extract_dh_pkey();

/* Step 8a: Client derives the secret key */
bool client_derive_secret_key();

/* Step 8a: Server derives the secret key */
bool server_derive_secret_key();

#endif