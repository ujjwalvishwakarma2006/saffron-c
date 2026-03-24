#ifndef KEY_EXCHANGE_H
#define KEY_EXCHANGE_H

/* ============================= KEY EXCHANGE PROTOCOL ============================= */
/* The protocol is executed on top of an established file channel and follows the    */
/* same call order for both peers on every startup session.                          */
/* ================================================================================= */

/* Utility to print a status line and refresh the target window. */
bool display_and_refresh(WINDOW* window, char* message);

/* Step 1a: Server sends certificate to client. */
bool server_send_certificate();

/* Step 1b: Client receives server certificate. */
bool client_recv_certificate();

/* Step 1c: Client verifies server certificate against root CA. */
bool client_verify_certificate();

/* Step 2a: Client sends certificate to server. */
bool client_send_certificate();

/* Step 2b: Server receives client certificate. */
bool server_recv_certificate();

/* Step 2c: Server verifies client certificate against root CA. */
bool server_verify_certificate();

/* Step 3a: Server generates shared DH parameters. */
bool server_generate_dh_params();

/* Step 3b: Server generates DH key pair and prepares DH packet. */
bool server_generate_dh_pkey();

/* Step 3c: Server signs DH packet with server RSA credentials. */
bool server_sign_dh_packet();

/* Step 3d: Server sends signed DH packet to client. */
bool server_send_signed_dh();

/* Step 4: Client receives signed DH packet. */
bool client_recv_signed_dh();

/* Step 5: Client verifies and extracts DH params + server DH public key. */
bool client_extract_dh_packet();

/* Step 5a: Client generates DH key pair from received parameters. */
bool client_generate_dh_pkey();

/* Step 5b: Client signs client DH public key. */
bool client_sign_dh_pkey();

/* Step 5c: Client sends signed DH public key to server. */
bool client_send_signed_dh_pkey();

/* Step 6: Server receives signed client DH public key. */
bool server_recv_signed_dh_pkey();

/* Step 7: Server verifies and extracts client DH public key. */
bool server_extract_dh_pkey();

/* Step 8a: Client derives shared session key. */
bool client_derive_secret_key();

/* Step 8b: Server derives shared session key. */
bool server_derive_secret_key();

/* Runs complete server-side key exchange sequence. */
bool run_server_key_exchange();

/* Runs complete client-side key exchange sequence. */
bool run_client_key_exchange();

#endif