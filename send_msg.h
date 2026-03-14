#ifndef SEND_MSG_H
#define SEND_MSG_H

/* Write message to temporary file */
void write_msg_file(char* message);

/* Encrypt the message file */
void encrypt_msg_file();

/* Sign encrypted message file */
void sign_msg_file(char* cert_path, char* skey_path);

/* Send (encrypted + signed) message */
void send_signed_enc_msg();

/* Display confirmation of sent message */
void display_sent_msg(char* message);

/* Use all functions above for more abstraction */
void send_msg(char* message);

#endif // !SEND_MSG_H