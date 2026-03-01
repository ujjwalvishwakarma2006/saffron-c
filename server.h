#ifndef SERVER_H
#define SERVER_H

int server_start(const char* server_ip, const int server_port, const char* label);
int server_accept(const int listen_fd, const char* channel_name);

#endif // !SERVER_H