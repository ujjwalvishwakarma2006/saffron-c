#include "common.h"
#include "send_msg.h"
#include "crypto.h"
#include "tui.h"
#include "send.h"
#include "file_utils.h"

/* Write message to temporary file */
void write_msg_file(char* message) {
    FILE* fp;

    fp = open_file(msg_out_path, "w");
    if (fp == NULL) return;
    fprintf(fp, "%s", message);

    fclose(fp);
}

/* Encrypt the message file */
void encrypt_msg_file() {
    encrypt_file(msg_out_path, msg_out_enc_path, session_key_path);
}

/* Sign encrypted message file */
void sign_msg_file(char* cert_path, char* skey_path) {
    cms_sign_file(msg_out_enc_path, cert_path, skey_path, msg_out_signed_path);
}

/* Send (encrypted + signed) message */
void send_signed_enc_msg() {
    send_file_content(msg_socket, msg_out_signed_path, buf_out);
}

/* Display confirmation of sent message */
void display_sent_msg(char* message) {
    sem_wait(&printing);
    wattron(log_win, COLOR_PAIR(CP_GREEN) | A_BOLD);
    wprintw(log_win, "You:");
    wattroff(log_win, COLOR_PAIR(CP_GREEN) | A_BOLD);
    wprintw(log_win, " %s\n", message);
    wrefresh(log_win);
    sem_post(&printing);
}

/* Use all functions above for more abstraction */
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