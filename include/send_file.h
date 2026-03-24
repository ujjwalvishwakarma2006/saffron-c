#ifndef SEND_FILE_H
#define SEND_FILE_H

/* Sends filename metadata ahead of encrypted file content. */
bool send_filename(char* file_path);

/* Displays file-sent confirmation in the chat log. */
void display_file_sent_confirmation(char* file_path);

/* End-to-end outgoing file pipeline:
 * send filename -> encrypt -> sign -> send payload -> display */
void send_file(char* file_path);

#endif // !SEND_FILE_H