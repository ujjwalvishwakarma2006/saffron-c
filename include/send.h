#ifndef SEND_H
#define SEND_H

/* Send content of filepath to the connection socket */
void send_file_content(int connection_socket, char* filepath, char* buffer);

#endif // !SEND_H