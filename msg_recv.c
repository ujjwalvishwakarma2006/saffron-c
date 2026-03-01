#include "common.h"
#include "crypto.h"
#include "tui.h"
#include "msg_recv.h"

void* msg_recv() {
    const int connection_socket = msg_socket;
    int n;
    uint32_t msg_len;
    uint32_t bytes_received, to_receive;
    FILE* fp;
    
    while (1) {
        // Clear buffer before receiving any message
        memset(msg_buf_in, 0, SIZE);

        // STEP 1: Receive message length
        n = recv(connection_socket, &msg_len, sizeof(msg_len), MSG_WAITALL);
        if (n <= 0) fatal_error("[MESSAGE LENGTH RECV ERROR]");

        // STEP 2: Open temp file to store the incoming message
        fp = fopen(msg_in_enc_path, "wb");
        if (fp == NULL) fatal_error("[MESSAGE FILE OPENING ERROR]");

        // STEP 3: Receive EXACT MESSAGE
        bytes_received = 0;
        while (bytes_received < msg_len) {
            to_receive = (msg_len-bytes_received > SIZE) ?
                        SIZE : (msg_len-bytes_received);
            n = recv(connection_socket, msg_buf_in, to_receive, MSG_WAITALL);
            if (n <= 0) {
                printf("[WARNING] Connection closed while receiving file\n");
                break;
            }

            // Write message chunk to the file
            fwrite(msg_buf_in, n, 1, fp);
            bytes_received += n;
        }

        // close file before decrypting, otherwise we'll get error
        fclose(fp);
        
        decrypt_file(msg_in_enc_path, msg_in_path, sym_key_path);

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
        usleep(100000);
    }
    return NULL;
}