#include "common.h"
#include "crypto.h"
#include "tui.h"
#include "send.h"

void send_filename(char* filename) {
    int n;
    int connection_socket = file_socket;
    uint32_t filename_len; 

    // Send filename length
    filename_len = strlen(filename);
    n = send(connection_socket, &filename_len, sizeof(filename_len), 0);
    if (n == -1) fatal_error("[FILENAME LENGTH SEND ERROR]");

    // Send the filename
    n = send(connection_socket, filename, filename_len, 0);
    if (n == -1) fatal_error("[FILENAME SEND ERROR]");
}

void file_encrypt(char* filename) {
    encrypt_file(filename, file_out_enc_path, sym_key_path);
}

void send_file_enc() {
    send_file_content(file_socket, file_out_enc_path, buf_out);
}

void send_file_tag() {
    
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
    send_filename(filename);
    file_encrypt(filename);
    send_file_enc();
    confirm_sent(filename);
}