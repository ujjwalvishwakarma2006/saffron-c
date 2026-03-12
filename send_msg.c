#include "common.h"
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

void msg_file_encrypt() {
    encrypt_file(msg_out_path, msg_out_enc_path, sym_key_path);
}

void send_msg_enc() {
    send_file_content(msg_socket, msg_out_enc_path, buf_out);
}

void send_msg_tag() {
    generate_hmac(msg_out_tag_path, msg_out_enc_path);
    send_file_content(msg_socket, msg_out_tag_path, buf_out);
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
    msg_file_encrypt();
    send_msg_enc();
    send_msg_tag();
    display_sent_msg(message);
    
}
