#include "common.h"
#include "crypto.h"
#include "tui.h"
#include "recv.h"
#include "recv_msg.h"

void read_file_content(char* buffer, char* filename) {
    FILE* fp;
    uint32_t file_size;
    
    fp = fopen(filename, "r");
    if (fp == NULL) fatal_error("[ERROR OPENING FILE]");

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    fread(buffer, 1, file_size, fp);

    fclose(fp);
}

void recv_msg_enc() {
    recv_file_content(msg_socket, msg_in_enc_path, msg_buf_in);
}

void recv_msg_tag() {
    recv_file_content(msg_socket, msg_in_recvd_tag_path, msg_buf_in);
}

bool vrfy_msg_tag() {
    generate_hmac(msg_in_local_tag_path, msg_in_enc_path);

    // Read locally generated HMAC
    read_file_content(local_msg_tag, msg_in_local_tag_path);
    
    // Read received HMAC
    read_file_content(recvd_msg_tag, msg_in_recvd_tag_path);

    // Return true if both HMACs match
    if (strncmp(local_msg_tag, recvd_msg_tag, TAG_SIZE) != 0) return true;

    return false;
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
        recv_msg_tag();
        // Do not show the message to the user, if integrit isn't verified
        if (vrfy_msg_tag() == false) continue;
        msg_decrypt();
        msg_display();

        usleep(100000);
    }
    return NULL;
}