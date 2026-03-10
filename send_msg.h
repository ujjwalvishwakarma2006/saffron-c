#ifndef SEND_MSG_H
#define SEND_MSG_H

void write_msg_file(char* message);         /* Write message to temporary file */
void msg_file_encrypt();                    /* Encrypt the message file */
void send_msg_enc();                        /* Send encrypted message file */
void send_msg_tag();                        /* Send HMAC for the file sent for both file_send and msg_send */
void display_sent_msg(char* message);       /* Display confirmation of sent message */

void send_msg(char* message);               /* Use all functions above for more abstraction */

#endif // !SEND_MSG_H