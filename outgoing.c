#include "common.h"
#include "crypto.h"
#include "tui.h"
#include "outgoing.h"
#include "send_file.h"
#include "send_msg.h"

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
            send_file(input + 3);
        } else {
            send_msg(input);
        }
     
        free(input);
    }
}