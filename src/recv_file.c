#include "common.h"
#include "crypto.h"
#include "tui.h"
#include "recv.h"
#include "recv_file.h"

/* Receive filename in the buffer */
void recv_filename(char* buffer, int size) {
    int n;
    uint32_t filename_len;

    // Clear buffer before receiving any object
    memset(buffer, 0, size);

    // Receive length of the filename first
    n = recv(file_socket, &filename_len, sizeof(filename_len), MSG_WAITALL);
    if (n <= 0) fatal_error("[FILENAME LENGTH RECV ERROR]");

    n = recv(file_socket, buffer, filename_len, MSG_WAITALL);
    if (n <= 0) fatal_error("[FILENAME RECV ERROR]");
    buffer[filename_len] = '\0';      /* Ensure null termination */
}

/* Receive encrypted file */
void recv_signed_file() {
    recv_file_content(file_socket, file_in_signed_path, file_buf_in);
    cms_extract_file(file_in_signed_path, root_ca_cert_path, file_in_enc_path);
}

/* Decrypt the file to actual filename */
void file_decrypt(char* filename) {
    decrypt_file(file_in_enc_path, filename, session_key_path);
}

/* Display file received confirmation message */
void confirm_recv(char* filename) {
    sem_wait(&printing);
    wattron(log_win, COLOR_PAIR(CP_YELLOW));
    wprintw(log_win, "\u2193 ");
    wattroff(log_win, COLOR_PAIR(CP_YELLOW));
    wattron(log_win, COLOR_PAIR(CP_MAGENTA) | A_BOLD);
    wprintw(log_win, "%s", display_name);
    wattroff(log_win, COLOR_PAIR(CP_MAGENTA) | A_BOLD);
    wattron(log_win, COLOR_PAIR(CP_YELLOW));
    wprintw(log_win, " sent ");
    wattroff(log_win, COLOR_PAIR(CP_YELLOW));
    wattron(log_win, COLOR_PAIR(CP_CYAN) | A_BOLD);
    wprintw(log_win, "`%s`", filename);
    wattroff(log_win, COLOR_PAIR(CP_CYAN) | A_BOLD);
    wprintw(log_win, "\n");
    wrefresh(log_win);
    sem_post(&printing);
}

/* Thread function: receive-verify-decrypt-display in loop*/
void* file_recv_loop(void*) {
    char filename[128];
    
    while (1) {
        recv_filename(filename, 128);
        recv_signed_file();
        file_decrypt(filename);
        confirm_recv(filename);

        usleep(100000);
    }

    return NULL;
}
