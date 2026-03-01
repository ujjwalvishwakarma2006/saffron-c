#include "common.h"
#include "crypto.h"
#include "tui.h"
#include "outgoing.h"

// ===============================================================================================
//                                    INPUT HANDLING FUNCTIONS 
// ===============================================================================================

void file_send(const int connection_socket, char* filename, const bool is_msg_file) {
    int n;
    FILE* fp;
    uint32_t filename_len;
    uint32_t file_size;

    // Clean filename [Skipping for now, causes seg fault when sending message]
    // filename[strcspn(filename, "\n")] = '\0';
    // filename[strcspn(filename, "\r")] = '\0';
    
    if (!is_msg_file) {
        // First, encrypt the file, and store the encrypted version out_file_filename_enc
        // Message file is already encrypted by msg_send() function
        encrypt_file(filename, file_out_enc_path, sym_key_path);
        
        // STEP 1: Send filename length first
        filename_len = strlen(filename);
        n = send(connection_socket, &filename_len, sizeof(filename_len), 0);
        if (n == -1) fatal_error("[FILENAME LENGTH SEND ERROR]");
        
        // STEP 2: Send the filename
        n = send(connection_socket, filename, filename_len, 0);
        if (n == -1) fatal_error("[FILENAME SEND ERROR]");
        
        // Open the encrypted file, file may not exist, therefore return in that case
        fp = fopen(file_out_enc_path, "rb");
        if (fp == NULL) {
            perror("[FILE OPENING ERROR]");
            return;
        }
    } else {
        // Open the encrypted message file, file may not exist, therefore return in that case
        fp = fopen(msg_out_enc_path, "rb");
        if (fp == NULL) {
            perror("[FILE OPENING ERROR]");
            return;
        }
    }
    
    // STEP 3: Get file size, and send to the server
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    n = send(connection_socket, &file_size, sizeof(file_size), 0);
    if (n == -1) fatal_error("[FILE LENGTH SEND ERROR]");

    // STEP 4: Send the file, and close fp
    while ((n = fread(buf_out, 1, SIZE, fp)) > 0) {
        send(connection_socket, buf_out, n, 0);
    }

    fclose(fp);
    if (!is_msg_file) {
        sem_wait(&printing);
        wattron(log_win, COLOR_PAIR(CP_YELLOW));
        wprintw(log_win, "\u2191 File sent ");
        wattroff(log_win, COLOR_PAIR(CP_YELLOW));
        wattron(log_win, COLOR_PAIR(CP_CYAN) | A_BOLD);
        wprintw(log_win, "`%s`", filename);
        wattroff(log_win, COLOR_PAIR(CP_CYAN) | A_BOLD);
        wprintw(log_win, "\n");
        wrefresh(log_win);
        sem_post(&printing);
    }
}

void msg_send(const int connection_socket, char* message) {
    FILE* fp;

    // First write the message into a file, will be useful furing encryption
    fp = fopen(msg_out_path, "w");
    if (fp == NULL) fatal_error("[MESSAGE FILE OPENING ERROR]");
    
    fprintf(fp, "%s", message);
    fclose(fp);

    encrypt_file(msg_out_path, msg_out_enc_path, sym_key_path);
    
    file_send(connection_socket, msg_out_enc_path, true);
}

// Function to take input with dynamic string length 
char* wgetstring(WINDOW* window) {
    int capacity = 512;
    char* buffer = malloc(capacity * sizeof(char));
    if (!buffer) return NULL;

    int ch;
    int i;

    for (i = 0; ; ++i) {
        ch = wgetch(window);
        if (ch == '\n') break;

        // Manually handle backspace behaviour
        if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (i > 0) {
                --i;
                int y, x;
                getyx(window, y, x);
                wmove(window, y, x - 1);
                wdelch(window);
                wrefresh(window);
            }
            --i;
            continue;
        }

        // Adjust buffer size based on the input length
        if (i >= capacity - 1) {
            capacity *= 2;
            char* new_buffer = realloc(buffer, capacity * sizeof(char));
            if (!new_buffer) {
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }
        buffer[i] = ch;
        waddch(window, ch);
        wrefresh(window);
    }

    buffer[i] = '\0';
    return buffer;
}

// Function to handle outgoing messages/files
void handle_outgoing(const int msg_socket, const int file_socket) {
    while (1) {
        char* input = wgetstring(input_win);
        wclear(input_win);
        wrefresh(input_win);
        
        if (!input) continue;
        
        if (strcmp(input, "/q") == 0) {
            free(input);
            close(msg_socket);
            close(file_socket);
            endwin();
            exit(0);
        } else if (strncmp(input, "/f ", 3) == 0) {
            file_send(file_socket, input + 3, false);
        } else {
            sem_wait(&printing);
            wattron(log_win, COLOR_PAIR(CP_GREEN) | A_BOLD);
            wprintw(log_win, "You:");
            wattroff(log_win, COLOR_PAIR(CP_GREEN) | A_BOLD);
            wprintw(log_win, " %s\n", input);
            wrefresh(log_win);
            msg_send(msg_socket, input);
            sem_post(&printing);
        }
     
        free(input);
    }
}