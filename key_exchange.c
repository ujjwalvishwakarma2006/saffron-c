#include "common.h"
#include "key_exchange.h"
#include "send.h"
#include "recv.h"
#include "crypto.h"
#include "file_utils.h"

/* Display and refresh after printing to the window */
void display_and_refresh(WINDOW* window, char* message) {
    sem_wait(&printing);
    wprintw(window, "%s", message);
    sem_post(&printing);
    wrefresh(window);
}

/* Step 1a: Server sends its certificate to the client */
void server_send_certificate() {
    send_file_content(file_socket, server_cert_path, buf_out);
    display_and_refresh(log_win, "[1] Certificate Sent\n");
}

/* Step 1b: Client receives the certificate */
void client_recv_certificate() {
    recv_file_content(file_socket, server_cert_path, file_buf_in);
    display_and_refresh(log_win, "[2] Server Certificate Received\n");
}

/* Step 1c: Client verifies server's certificate against root CA's certificate */
void client_verify_certificate() {
    verify_certificate(root_ca_cert_path, server_cert_path);
    display_and_refresh(log_win, "[3] Server certificate verified successfully\n");
}

/* Step 2a: Client sends its certificate to the server */
void client_send_certificate() {
    send_file_content(file_socket, client_cert_path, buf_out);
    display_and_refresh(log_win, "[4] Certificate Sent\n");
}

/* Step 2b: Server receives the certificate */
void server_recv_certificate() {
    recv_file_content(file_socket, client_cert_path, file_buf_in);
    display_and_refresh(log_win, "[5] Client Certificate Received\n");
}

/* Step 2c: Server verifies client's certificate against root CA's certificate */
void server_verify_certificate() {
    verify_certificate(root_ca_cert_path, client_cert_path);
    display_and_refresh(log_win, "[6] Client certificate verified successfully\n");
}

/* Step 3a: Server generates Diffie-Hellman parameters */
void server_generate_dh_params() {
    generate_dh_params(dh_param_path);
}

/* Step 3b: Server generates its public key and appends in the dh_param file */
void server_generate_dh_pkey() {
    generate_dh_key_pair(dh_param_path, server_dh_skey_path, server_dh_pkey_path);
    char* file_names[] = {dh_param_path, server_dh_pkey_path};
    merge_files(2, file_names, file_out_path);
}

/* Step 3c: Server signs Diffie-Hellman packet with it's RSA private key */
void server_sign_dh_packet() {
    cms_sign_file(file_out_path, server_cert_path, server_skey_path, file_out_signed_path);
}

/* Step 3d: Server sends signed DH packet to the client */
void server_send_signed_dh() {
    send_file_content(file_socket, file_out_signed_path, buf_out);
    display_and_refresh(log_win, "[7] DH Parameters + DH PKEY sent\n");
}

/* Step 4: Client receives the signed DH packet */
void client_recv_signed_dh() {
    recv_file_content(file_socket, file_in_signed_path, file_buf_in);
}

/* Step 5: Client extracts the contents of signed DH packet if the signature is valid */
void client_extract_dh_packet() {
    cms_extract_file(file_in_signed_path, root_ca_cert_path, file_in_path);
    char* out_files[] = {dh_param_path, server_dh_pkey_path};
    split_file(file_in_path, 2, out_files);
    display_and_refresh(log_win, "[8] DH Parameters + Server DH PKEY received\n");
}

/* Step 5a: Client generates its public key based on received DH parameters */
void client_generate_dh_pkey() {
    generate_dh_key_pair(dh_param_path, client_dh_skey_path, client_dh_pkey_path);
}

/* Step 5b: Client signs its public key with its RSA private key */
void client_sign_dh_pkey() {
    cms_sign_file(client_dh_pkey_path, client_cert_path, client_skey_path, file_out_signed_path);
}

/* Step 5c: Client sends its signed public key to the server */
void client_send_signed_dh_pkey() {
    send_file_content(file_socket, file_out_signed_path, buf_out);
    display_and_refresh(log_win, "[9] DH PKEY sent\n");
}

/* Step 6: Server receives signed public key from the client */
void server_recv_signed_dh_pkey() {
    recv_file_content(file_socket, file_in_signed_path, file_buf_in);
}

/* Step 7: Server extracts client's public key if it's signature is verified */
void server_extract_dh_pkey() {
    cms_extract_file(file_in_signed_path, root_ca_cert_path, client_dh_pkey_path);
    display_and_refresh(log_win, "[10] Client PKEY received\n");
}

/* Step 8a: Client derives DH secret key */
void client_derive_secret_key() {
    derive_dh_skey(client_dh_skey_path, server_dh_pkey_path, session_key_path);
    display_and_refresh(log_win, "===========[ Secret Key derived ]===========\n");
}

/* Step 8a: Server derives DH secret key */
void server_derive_secret_key() {
    derive_dh_skey(server_dh_skey_path, client_dh_pkey_path, session_key_path);
    display_and_refresh(log_win, "===========[ Secret Key derived ]===========\n");
}