#include "common.h"
#include "send_file.h"
#include "crypto.h"
#include "tui.h"
#include "send.h"

bool send_filename(char* filename) {
    // If there is error in opening file, return false;
    if (access(filename, F_OK) != 0) {
        sem_wait(&printing);
        wattron(log_win, COLOR_PAIR(CP_RED));
        wprintw(log_win, "File `%s` doesn't exist.\n", filename);
        wattroff(log_win, COLOR_PAIR(CP_RED));
        sem_post(&printing);
        wrefresh(log_win);
        return false; 
    }

    int n;
    uint32_t filename_len; 

    // Send filename length
    filename_len = strlen(filename);
    n = send(file_socket, &filename_len, sizeof(filename_len), 0);
    if (n == -1) fatal_error("[FILENAME LENGTH SEND ERROR]");

    // Send the filename
    n = send(file_socket, filename, filename_len, 0);
    if (n == -1) fatal_error("[FILENAME SEND ERROR]");

    return true;
}

void file_encrypt(char* filename) {
    encrypt_file(filename, file_out_enc_path, session_key_path);
}

void sign_file(char* cert_path, char* skey_path) {
    cms_sign_file(file_out_enc_path, cert_path, skey_path, file_out_signed_path);
}

void send_signed_file() {
    send_file_content(file_socket, file_out_signed_path, buf_out);
}

void confirm_sent(char* filename) {
    sem_wait(&printing);
    wattron(log_win, COLOR_PAIR(CP_YELLOW));
    wprintw(log_win, "\u2191 File sent ");
    wattroff(log_win, COLOR_PAIR(CP_YELLOW));
    wattron(log_win, COLOR_PAIR(CP_CYAN) | A_BOLD);
    wprintw(log_win, "`%s`", filename);
    wattroff(log_win, COLOR_PAIR(CP_CYAN) | A_BOLD);
    wprintw(log_win, "\n");
    wrefresh(log_win);
    sem_post(&printing);
}

void send_file(char* filename) {
    if (!send_filename(filename)) return;
    file_encrypt(filename);
    if (app_mode == CLIENT) sign_file(client_cert_path, client_skey_path);
    else if (app_mode == SERVER) sign_file(server_cert_path, server_skey_path);
    send_signed_file();
    confirm_sent(filename);
}