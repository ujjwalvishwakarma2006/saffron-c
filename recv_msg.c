#include "common.h"
#include "crypto.h"
#include "tui.h"
#include "recv.h"
#include "recv_msg.h"

void recv_msg_enc() {
    recv_file_content(msg_socket, msg_in_enc_path, msg_buf_in);
}

void recv_msg_tag() {
    
}

void vrfy_msg_tag() {
    
}

void msg_decrypt() {
    decrypt_file(msg_in_enc_path, msg_in_path, sym_key_path);
}

void msg_display() {
    FILE* fp;
    
    fp = fopen(msg_in_path, "r");
    if (fp == NULL) fatal_error("[RECV_MSG FILE OPENING ERROR]");
    
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


void* msg_recv_loop() {
    while (1) {
        recv_msg_enc();
        msg_decrypt();
        msg_display();

        usleep(100000);
    }
    return NULL;
}