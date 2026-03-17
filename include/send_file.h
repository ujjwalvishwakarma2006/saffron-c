#ifndef SEND_FILE_H
#define SEND_FILE_H

/* Send filename */
bool send_filename(char* filename);

/* Encrypt outgoing file */
void file_encrypt(char* filename);

/* Sign Encrypted file */
void sign_file(char* cert_path, char* skey_path);

/* Send (signed + encrypted) file integrity */
void send_signed_file();

/* Display file sent confirmation message */
void confirm_sent(char* filename);

/* Uses all functions above for more abstraction*/
void send_file(char* filename);

#endif // !SEND_FILE_H