#include "common.h"
#include "crypto.h"
#include "tui.h"
#include "recv.h"
#include "recv_msg.h"
#include "file_utils.h"

void recv_signed_enc_msg() {
    recv_file_content(msg_socket, msg_in_signed_path, msg_buf_in);
}

void extract_enc_msg() {
    cms_extract_file(msg_in_signed_path, root_ca_cert_path, msg_in_enc_path);
}

void msg_decrypt() {
    decrypt_file(msg_in_enc_path, msg_in_path, session_key_path);
}

void msg_display() {
    FILE* fp;
    
    fp = open_file(msg_in_path, "r");
    if (fp == NULL) return;
    
    sem_wait(&printing);

    wattron(log_win, COLOR_PAIR(CP_MAGENTA) | A_BOLD);
    wprintw(log_win, "%s:", display_name);
    wattroff(log_win, COLOR_PAIR(CP_MAGENTA) | A_BOLD);
    wprintw(log_win, " ");

    while (fgets(msg_buf_in, sizeof(msg_buf_in), fp) != NULL) {
        wprintw(log_win, "%s", msg_buf_in);
    }

    wprintw(log_win, "\n");
    wrefresh(log_win);

    sem_post(&printing);
    fclose(fp);
}

void* msg_recv_loop(void*) {
    while (1) {
        recv_signed_enc_msg();
        extract_enc_msg();
        msg_decrypt();
        msg_display();
        usleep(100000);
    }
    return NULL;
}