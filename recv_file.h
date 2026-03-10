#ifndef FILE_RECV_H
#define FILE_RECV_H

void recv_filename(char* buffer);       /* Receive filename in the buffer */
void recv_file_enc();                   /* Receive encrypted file */
void recv_file_tag();                   /* Receive HMAC tag of the file */
void vrfy_file_tag();                   /* Verify file tag */
void file_decrypt(char* filename);      /* Decrypt the file to actual filename */
void confirm_recv(char* filename);      /* Display file received confirmation message */

void* file_recv_loop();                 /* Thread function: receive-verify-decrypt-display in loop*/

#endif // !FILE_RECV_H