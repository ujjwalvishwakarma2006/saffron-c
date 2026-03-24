#include "common.h"
#include "send_file.h"
#include "crypto.h"
#include "tui.h"
#include "send.h"
#include "file_utils.h"

/* Send filename metadata before sending encrypted file payload. */
bool send_filename(char* file_path) {
    if (!can_access(file_path)) return false;

    int bytes_sent;
    uint32_t file_name_length;

    file_name_length = strlen(file_path);
    bytes_sent = send(file_socket, &file_name_length, sizeof(file_name_length), 0);
    if (bytes_sent == -1) fatal_error("[FILENAME LENGTH SEND ERROR]");

    bytes_sent = send(file_socket, file_path, file_name_length, 0);
    if (bytes_sent == -1) fatal_error("[FILENAME SEND ERROR]");

    return true;
}

/* Display local confirmation that file transfer was initiated successfully. */
void display_file_sent_confirmation(char* file_path) {
    sem_wait(&printing);
    wattron(log_win, COLOR_PAIR(CP_YELLOW));
    wprintw(log_win, "\u2191 File sent ");
    wattroff(log_win, COLOR_PAIR(CP_YELLOW));
    wattron(log_win, COLOR_PAIR(CP_CYAN) | A_BOLD);
    wprintw(log_win, "`%s`", file_path);
    wattroff(log_win, COLOR_PAIR(CP_CYAN) | A_BOLD);
    wprintw(log_win, "\n");
    wrefresh(log_win);
    sem_post(&printing);
}

/* Complete outgoing file pipeline with role-specific signing credentials. */
void send_file(char* file_path) {
    if (!send_filename(file_path)) return;

    encrypt_file(file_path, file_out_enc_path, session_key_path);

    if (app_mode == CLIENT) {
        cms_sign_file(file_out_enc_path, client_cert_path, client_secret_key_path, file_out_signed_path);
    } else if (app_mode == SERVER) {
        cms_sign_file(file_out_enc_path, server_cert_path, server_secret_key_path, file_out_signed_path);
    }

    send_file_content(file_socket, file_out_signed_path, output_buffer);
    display_file_sent_confirmation(file_path);
}