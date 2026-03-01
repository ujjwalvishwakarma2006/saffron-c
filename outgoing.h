#ifndef OUTGOING_H
#define OUTGOING_H

void file_send(const int connection_socket, char* filename, const bool is_msg_file);

void msg_send(const int connection_socket, char* message)

char* wgetstring(WINDOW* window);

void handle_outgoing(int msg_socket, int file_socket);

#endif // !OUTGOING_H