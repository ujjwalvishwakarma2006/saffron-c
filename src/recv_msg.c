#include "common.h"
#include "crypto.h"
#include "tui.h"
#include "recv.h"
#include "recv_msg.h"
#include "file_utils.h"

/* Display one decrypted incoming message file in the chat window. */
void display_received_message() {
    FILE* message_file = open_file(msg_in_path, "r");
    if (message_file == NULL) return;

    sem_wait(&printing);

    wattron(log_win, COLOR_PAIR(CP_MAGENTA) | A_BOLD);
    wprintw(log_win, "%s:", peer_display_name);
    wattroff(log_win, COLOR_PAIR(CP_MAGENTA) | A_BOLD);
    wprintw(log_win, " ");

    while (fgets(message_input_buffer, sizeof(message_input_buffer), message_file) != NULL) {
        wprintw(log_win, "%s", message_input_buffer);
    }

    wprintw(log_win, "\n");
    wrefresh(log_win);

    sem_post(&printing);
    fclose(message_file);
}

/* Receive, verify, decrypt, and display messages forever on a dedicated receiver thread. */
void* msg_recv_loop(void*) {
    while (1) {
        recv_file_content(msg_socket, msg_in_signed_path, message_input_buffer);
        cms_extract_file(msg_in_signed_path, root_ca_cert_path, msg_in_enc_path);
        decrypt_file(msg_in_enc_path, msg_in_path, session_key_path);
        display_received_message();
        usleep(100000);
    }

    return NULL;
}