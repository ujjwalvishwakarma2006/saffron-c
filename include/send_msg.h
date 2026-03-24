#ifndef SEND_MSG_H
#define SEND_MSG_H

/* Writes outgoing message text to the plaintext temporary message file. */
void write_message_to_temp_file(char* message_text);

/* Displays confirmation of a sent message in the log window. */
void display_sent_message(char* message_text);

/* End-to-end outgoing message pipeline:
 * write -> encrypt -> sign -> send -> display */
void send_msg(char* message_text);

#endif // !SEND_MSG_H