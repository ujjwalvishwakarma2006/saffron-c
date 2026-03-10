#ifndef SEND_FILE_H
#define SEND_FILE_H

void send_filename(char* filename);         /* Send filename */
void file_encrypt(char* filename);          /* Encrypt outgoing file */
void send_file_enc();                       /* Send Encrypted file */
void send_file_tag();                       /* Send HMAC for data integrity */
void confirm_sent(char* filename);          /* Display file sent confirmation message */

void send_file(char* filename);             /* Uses all functions above for more abstraction*/

#endif // !SEND_FILE_H