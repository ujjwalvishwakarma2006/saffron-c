#include "common.h"
#include "send_msg.h"
#include "crypto.h"
#include "tui.h"
#include "send.h"

void write_msg_file(char* message) {
    FILE* fp;

    fp = fopen(msg_out_path, "w");
    if (fp == NULL) fatal_error("[MESSAGE FILE OPENING ERROR]");
    fprintf(fp, "%s", message);

    fclose(fp);
}

void encrypt_msg_file() {
    encrypt_file(msg_out_path, msg_out_enc_path, session_key_path);
}

void sign_msg_file(char* cert_path, char* skey_path) {
    cms_sign_file(msg_out_enc_path, cert_path, skey_path, msg_out_signed_path);
}

void send_signed_enc_msg() {
    send_file_content(msg_socket, msg_out_signed_path, buf_out);
}

void display_sent_msg(char* message) {
    sem_wait(&printing);
    wattron(log_win, COLOR_PAIR(CP_GREEN) | A_BOLD);
    wprintw(log_win, "You:");
    wattroff(log_win, COLOR_PAIR(CP_GREEN) | A_BOLD);
    wprintw(log_win, " %s\n", message);
    wrefresh(log_win);
    sem_post(&printing);
}

void send_msg(char* message) {

    write_msg_file(message);
    encrypt_msg_file();
    if (app_mode == CLIENT) {
        sign_msg_file(client_cert_path, client_skey_path);
    } else if (app_mode == SERVER) {
        sign_msg_file(server_cert_path, server_skey_path);
    }
    send_signed_enc_msg();
    display_sent_msg(message);
    
}