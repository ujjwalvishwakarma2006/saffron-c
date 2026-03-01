#include "common.h"
#include "crypto.h"
#include "tui.h"
#include "file_recv.h"

void* file_recv() {
    const int connection_socket = file_socket;
    int n;
    char filename[128];
    FILE* fp;
    uint32_t filename_len;
    uint32_t file_size;
    uint32_t bytes_received, to_receive;
    
    while (1) {
        // Clear buffers before receiving any object
        memset(filename, 0, sizeof(filename));

        // STEP 1: Receive length of the filename first
        n = recv(connection_socket, &filename_len, sizeof(filename_len), MSG_WAITALL);
        if (n <= 0) fatal_error("[FILENAME LENGTH RECV ERROR]");

        // STEP 2: Receive EXACT filename based on the length
        n = recv(connection_socket, filename, filename_len, MSG_WAITALL);
        if (n <= 0) fatal_error("[FILENAME RECV ERROR]");
        filename[filename_len] = '\0';     // Ensure null termination

        // STEP 3: Receive file size
        n = recv(connection_socket, &file_size, sizeof(file_size), MSG_WAITALL);
        if (n <= 0) fatal_error("[FILESIZE RECV ERROR]");

        // Validate filename
        if (strlen(filename) == 0) {
            perror("[EMPTY FILENAME ERROR]");
            continue;
        }

        // Open file for writing
        fp = fopen(file_in_enc_path, "wb");
        if (fp == NULL) {
            perror("[FILE OPENING ERROR]");
            continue;   // Continue to receive other files
        }

        // STEP 4: Receive exact file_size many bytes
        bytes_received = 0;
        while (bytes_received < file_size) {
            to_receive = (file_size-bytes_received > SIZE) ? 
                        SIZE : (file_size-bytes_received);
                        
            n = recv(connection_socket, file_buf_in, to_receive, 0);
            if (n <= 0) {
                printf("[WARNING] Connection closed while receiving file\n");
                break;
            }
            
            // Write to file
            fwrite(file_buf_in, 1, n, fp);
            bytes_received += n;
        }

        // close file before decrypting, otherwise we'll get error
        fclose(fp);

        decrypt_file(file_in_enc_path, filename, sym_key_path);

        sem_wait(&printing);
        wattron(log_win, COLOR_PAIR(CP_YELLOW));
        wprintw(log_win, "\u2193 ");
        wattroff(log_win, COLOR_PAIR(CP_YELLOW));
        wattron(log_win, COLOR_PAIR(CP_MAGENTA) | A_BOLD);
        wprintw(log_win, "%s", display_name);
        wattroff(log_win, COLOR_PAIR(CP_MAGENTA) | A_BOLD);
        wattron(log_win, COLOR_PAIR(CP_YELLOW));
        wprintw(log_win, " sent ");
        wattroff(log_win, COLOR_PAIR(CP_YELLOW));
        wattron(log_win, COLOR_PAIR(CP_CYAN) | A_BOLD);
        wprintw(log_win, "`%s`", filename);
        wattroff(log_win, COLOR_PAIR(CP_CYAN) | A_BOLD);
        wprintw(log_win, "\n");
        wrefresh(log_win);
        sem_post(&printing);
        usleep(100000);
    }

    return NULL;
}
