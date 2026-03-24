#include "common.h"
#include "crypto.h"
#include "tui.h"
#include "recv.h"
#include "recv_file.h"

/* Receive filename metadata sent before each file payload. */
void recv_filename(char* output_buffer, int size) {
    int bytes_received;
    uint32_t file_name_length;

    memset(output_buffer, 0, size);

    bytes_received = recv(file_socket, &file_name_length, sizeof(file_name_length), MSG_WAITALL);
    if (bytes_received <= 0) fatal_error("[FILENAME LENGTH RECV ERROR]");

    bytes_received = recv(file_socket, output_buffer, file_name_length, MSG_WAITALL);
    if (bytes_received <= 0) fatal_error("[FILENAME RECV ERROR]");

    output_buffer[file_name_length] = '\0';
}

/* Display incoming-file confirmation with sender and filename. */
void display_file_received_confirmation(char* file_path) {
    sem_wait(&printing);
    wattron(log_win, COLOR_PAIR(CP_YELLOW));
    wprintw(log_win, "\u2193 ");
    wattroff(log_win, COLOR_PAIR(CP_YELLOW));

    wattron(log_win, COLOR_PAIR(CP_MAGENTA) | A_BOLD);
    wprintw(log_win, "%s", peer_display_name);
    wattroff(log_win, COLOR_PAIR(CP_MAGENTA) | A_BOLD);

    wattron(log_win, COLOR_PAIR(CP_YELLOW));
    wprintw(log_win, " sent ");
    wattroff(log_win, COLOR_PAIR(CP_YELLOW));

    wattron(log_win, COLOR_PAIR(CP_CYAN) | A_BOLD);
    wprintw(log_win, "`%s`", file_path);
    wattroff(log_win, COLOR_PAIR(CP_CYAN) | A_BOLD);

    wprintw(log_win, "\n");
    wrefresh(log_win);
    sem_post(&printing);
}

/* Receive, verify, decrypt, and confirm incoming files on a dedicated receiver thread. */
void* file_recv_loop(void*) {
    char received_file_path[128];

    while (1) {
        recv_filename(received_file_path, 128);
        recv_file_content(file_socket, file_in_signed_path, file_input_buffer);
        cms_extract_file(file_in_signed_path, root_ca_cert_path, file_in_enc_path);
        decrypt_file(file_in_enc_path, received_file_path, session_key_path);
        display_file_received_confirmation(received_file_path);
        usleep(100000);
    }

    return NULL;
}
