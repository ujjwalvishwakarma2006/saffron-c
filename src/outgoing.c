#include "common.h"
#include "crypto.h"
#include "tui.h"
#include "outgoing.h"
#include "send_file.h"
#include "send_msg.h"

/* Reads one full line from the input window.
 * It supports backspace keys and doubles the backing buffer when needed. */
char* read_window_input(WINDOW* window) {
    int capacity = 512;
    char* input_buffer = malloc(capacity * sizeof(char));
    if (!input_buffer) return NULL;

    int key_input;
    int index;

    for (index = 0; ; ++index) {
        key_input = wgetch(window);
        if (key_input == '\n') break;

        if (key_input == KEY_BACKSPACE || key_input == 127 || key_input == 8) {
            if (index > 0) {
                --index;
                int row, col;
                getyx(window, row, col);
                wmove(window, row, col - 1);
                wdelch(window);
                wrefresh(window);
            }

            /* Counter the loop increment so next key stays at the correct index. */
            --index;
            continue;
        }

        if (index >= capacity - 1) {
            capacity *= 2;
            char* resized_buffer = realloc(input_buffer, capacity * sizeof(char));
            if (!resized_buffer) {
                free(input_buffer);
                return NULL;
            }
            input_buffer = resized_buffer;
        }

        input_buffer[index] = key_input;
        waddch(window, key_input);
        wrefresh(window);
    }

    input_buffer[index] = '\0';
    return input_buffer;
}

/* Handles outgoing commands:
 *   /q        -> quit
 *   /f <path> -> send file
 *   <text>    -> send message */
void handle_outgoing(const int msg_socket, const int file_socket) {
    while (1) {
        char* input = read_window_input(input_win);
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