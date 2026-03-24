#include "common.h"
#include "key_exchange.h"
#include "send.h"
#include "recv.h"
#include "crypto.h"
#include "file_utils.h"

/* Print one status line atomically and refresh the target window. */
bool display_and_refresh(WINDOW* window, char* message) {
    sem_wait(&printing);
    wprintw(window, "%s", message);
    sem_post(&printing);
    wrefresh(window);
    return true;
}

/* Step 1a: Server sends its certificate to the client. */
bool server_send_certificate() {
    send_file_content(file_socket, server_cert_path, output_buffer);
    return display_and_refresh(log_win, "[1] Server certificate sent\n");
}

/* Step 1b: Client receives server certificate. */
bool client_recv_certificate() {
    recv_file_content(file_socket, server_cert_path, file_input_buffer);
    return display_and_refresh(log_win, "[2] Server certificate received\n");
}

/* Step 1c: Client verifies server certificate against root CA. */
bool client_verify_certificate() {
    if (!verify_certificate(root_ca_cert_path, server_cert_path)) return false;
    return display_and_refresh(log_win, "[3] Server certificate verified\n");
}

/* Step 2a: Client sends its certificate to the server. */
bool client_send_certificate() {
    send_file_content(file_socket, client_cert_path, output_buffer);
    return display_and_refresh(log_win, "[4] Client certificate sent\n");
}

/* Step 2b: Server receives client certificate. */
bool server_recv_certificate() {
    recv_file_content(file_socket, client_cert_path, file_input_buffer);
    return display_and_refresh(log_win, "[5] Client certificate received\n");
}

/* Step 2c: Server verifies client certificate against root CA. */
bool server_verify_certificate() {
    if (!verify_certificate(root_ca_cert_path, client_cert_path)) return false;
    return display_and_refresh(log_win, "[6] Client certificate verified\n");
}

/* Step 3a: Server generates DH parameters. */
bool server_generate_dh_params() {
    return generate_dh_params(dh_params_path);
}

/* Step 3b: Server creates DH key pair and packs params + public key into one packet. */
bool server_generate_dh_pkey() {
    if (!generate_dh_key_pair(
            dh_params_path,
            server_dh_secret_key_path,
            server_dh_public_key_path)) {
        return false;
    }

    char* dh_packet_files[] = {dh_params_path, server_dh_public_key_path};
    return merge_files(2, dh_packet_files, file_out_path);
}

/* Step 3c: Server signs DH packet with server RSA private key. */
bool server_sign_dh_packet() {
    return cms_sign_file(
        file_out_path,
        server_cert_path,
        server_secret_key_path,
        file_out_signed_path
    );
}

/* Step 3d: Server sends signed DH packet to client. */
bool server_send_signed_dh() {
    send_file_content(file_socket, file_out_signed_path, output_buffer);
    return display_and_refresh(log_win, "[7] Signed DH packet sent\n");
}

/* Step 4: Client receives signed DH packet. */
bool client_recv_signed_dh() {
    recv_file_content(file_socket, file_in_signed_path, file_input_buffer);
    return true;
}

/* Step 5: Client verifies signed DH packet, then extracts params and server public key. */
bool client_extract_dh_packet() {
    if (!cms_extract_file(file_in_signed_path, root_ca_cert_path, file_in_path)) return false;

    char* extracted_files[] = {dh_params_path, server_dh_public_key_path};
    if (!split_file(file_in_path, 2, extracted_files)) return false;

    return display_and_refresh(log_win, "[8] DH params and server public key extracted\n");
}

/* Step 5a: Client generates DH key pair from received parameters. */
bool client_generate_dh_pkey() {
    return generate_dh_key_pair(
        dh_params_path,
        client_dh_secret_key_path,
        client_dh_public_key_path
    );
}

/* Step 5b: Client signs DH public key with client RSA private key. */
bool client_sign_dh_pkey() {
    return cms_sign_file(
        client_dh_public_key_path,
        client_cert_path,
        client_secret_key_path,
        file_out_signed_path
    );
}

/* Step 5c: Client sends signed DH public key to server. */
bool client_send_signed_dh_pkey() {
    send_file_content(file_socket, file_out_signed_path, output_buffer);
    return display_and_refresh(log_win, "[9] Signed client DH public key sent\n");
}

/* Step 6: Server receives signed client DH public key. */
bool server_recv_signed_dh_pkey() {
    recv_file_content(file_socket, file_in_signed_path, file_input_buffer);
    return true;
}

/* Step 7: Server verifies and extracts client DH public key. */
bool server_extract_dh_pkey() {
    if (!cms_extract_file(file_in_signed_path, root_ca_cert_path, client_dh_public_key_path)) {
        return false;
    }

    return display_and_refresh(log_win, "[10] Client DH public key extracted\n");
}

/* Step 8a: Client derives shared session key. */
bool client_derive_secret_key() {
    if (!derive_dh_skey(
            client_dh_secret_key_path,
            server_dh_public_key_path,
            session_key_path)) {
        return false;
    }

    return display_and_refresh(log_win, "===========[ Session key derived ]===========\n");
}

/* Step 8b: Server derives shared session key. */
bool server_derive_secret_key() {
    if (!derive_dh_skey(
            server_dh_secret_key_path,
            client_dh_public_key_path,
            session_key_path)) {
        return false;
    }

    return display_and_refresh(log_win, "===========[ Session key derived ]===========\n");
}

/* Execute complete server-side key exchange with step-specific failure context. */
bool run_server_key_exchange() {
    if (!server_send_certificate()) return false;
    if (!server_recv_certificate()) return false;
    if (!server_verify_certificate()) return false;
    if (!server_generate_dh_pkey()) return false;
    if (!server_sign_dh_packet()) return false;
    if (!server_send_signed_dh()) return false;
    if (!server_recv_signed_dh_pkey()) return false;
    if (!server_extract_dh_pkey()) return false;
    if (!server_derive_secret_key()) return false;
    return true;
}

/* Execute complete client-side key exchange with step-specific failure context. */
bool run_client_key_exchange() {
    if (!client_recv_certificate()) return false;
    if (!client_verify_certificate()) return false;
    if (!client_send_certificate()) return false;
    if (!client_recv_signed_dh()) return false;
    if (!client_extract_dh_packet()) return false;
    if (!client_generate_dh_pkey()) return false;
    if (!client_sign_dh_pkey()) return false;
    if (!client_send_signed_dh_pkey()) return false;
    if (!client_derive_secret_key()) return false;
    return true;
}