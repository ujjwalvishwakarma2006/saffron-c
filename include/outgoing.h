#ifndef OUTGOING_H
#define OUTGOING_H

char* wgetstring(WINDOW* window);

void handle_outgoing(int msg_socket, int file_socket);

#endif // !OUTGOING_H