#ifndef SEND_FILE_H
#define SEND_FILE_H

bool send_filename(char* filename);         /* Send filename */
void file_encrypt(char* filename);          /* Encrypt outgoing file */
void sign_file();                           /* Sign Encrypted file */
void send_signed_file();                    /* Send (signed + encrypted) file integrity */
void confirm_sent(char* filename);          /* Display file sent confirmation message */

void send_file(char* filename);             /* Uses all functions above for more abstraction*/

#endif // !SEND_FILE_H