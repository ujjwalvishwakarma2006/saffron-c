#ifndef FILE_RECV_H
#define FILE_RECV_H

/* Receives the next filename into output_buffer (must have size bytes). */
void recv_filename(char* output_buffer, int size);

/* Displays file-received confirmation in the chat log. */
void display_file_received_confirmation(char* file_path);

/* Thread loop for incoming file transfers:
 * receive filename -> receive signed payload -> verify -> decrypt -> display */
void* file_recv_loop(void*);

#endif // !FILE_RECV_H