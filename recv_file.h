#ifndef FILE_RECV_H
#define FILE_RECV_H

void recv_filename(char* buffer, int size); /* Receive filename in the buffer */
void recv_signed_file();                    /* Receive encrypted file */
void file_decrypt(char* filename);          /* Decrypt the file to actual filename */
void confirm_recv(char* filename);          /* Display file received confirmation message */

void* file_recv_loop(void*);                     /* Thread function: receive-verify-decrypt-display in loop*/

#endif // !FILE_RECV_H