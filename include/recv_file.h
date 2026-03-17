#ifndef FILE_RECV_H
#define FILE_RECV_H

/* Receive filename in the buffer */
void recv_filename(char* buffer, int size);

/* Receive encrypted file */
void recv_signed_file();

/* Decrypt the file to actual filename */
void file_decrypt(char* filename);

/* Display file received confirmation message */
void confirm_recv(char* filename);

/* Thread function: receive-verify-decrypt-display in loop*/
void* file_recv_loop(void*);

#endif // !FILE_RECV_H