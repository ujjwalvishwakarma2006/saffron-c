#ifndef OUTGOING_H
#define OUTGOING_H

/* Reads one input line from the given ncurses window with dynamic growth support. */
char* read_window_input(WINDOW* window);

/* Main loop for outgoing chat commands, messages, and file transfers. */
void handle_outgoing(int msg_socket, int file_socket);

#endif // !OUTGOING_H