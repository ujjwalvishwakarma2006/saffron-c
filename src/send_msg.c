#include "common.h"
#include "send_msg.h"
#include "crypto.h"
#include "tui.h"
#include "send.h"
#include "file_utils.h"

/* Write outgoing message text into the plaintext temp file consumed by the crypto pipeline. */
void write_message_to_temp_file(char* message_text) {
    FILE* message_file = open_file(msg_out_path, "w");
    if (message_file == NULL) return;

    fprintf(message_file, "%s", message_text);
    fclose(message_file);
}

/* Log sent message in local chat view. */
void display_sent_message(char* message_text) {
    sem_wait(&printing);
    wattron(log_win, COLOR_PAIR(CP_GREEN) | A_BOLD);
    wprintw(log_win, "You:");
    wattroff(log_win, COLOR_PAIR(CP_GREEN) | A_BOLD);
    wprintw(log_win, " %s\n", message_text);
    wrefresh(log_win);
    sem_post(&printing);
}

/* Complete outgoing message pipeline with role-specific signing credentials. */
void send_msg(char* message_text) {
    write_message_to_temp_file(message_text);

    encrypt_file(msg_out_path, msg_out_enc_path, session_key_path);

    if (app_mode == CLIENT) {
        cms_sign_file(msg_out_enc_path, client_cert_path, client_secret_key_path, msg_out_signed_path);
    } else if (app_mode == SERVER) {
        cms_sign_file(msg_out_enc_path, server_cert_path, server_secret_key_path, msg_out_signed_path);
    }

    send_file_content(msg_socket, msg_out_signed_path, output_buffer);
    display_sent_message(message_text);
}