#include "common.h"
#include "server.h"

// ===============================================================================================
//                                      SERVER SPECIFC FUNCTIONS 
// ===============================================================================================

int server_start(const char* server_ip, const int server_port, const char* label) {
    int server_socket;
    int n;
    struct sockaddr_in server_addr;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) fatal_error("[SOCKET INIT ERROR]");
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = server_port;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    // Bind socket to the port
    n = bind(server_socket, (struct sockaddr*)& server_addr, sizeof(server_addr));
    if (n == -1) fatal_error("[SOCKET BIND ERROR]");

    // Start listening on the port
    n = listen(server_socket, 2);
    if (n == -1) fatal_error("[PORT LISTEN ERROR]");
    wprintw(log_win, "Listening on port %d for %s Connection Requests\n", server_port, label);
    wrefresh(log_win);

    return server_socket;
}

int server_accept(const int listen_fd) {
    int connection_socket;
    struct sockaddr_in client_addr;
    socklen_t addr_size;
    
    // Accept incoming request
    connection_socket = accept(listen_fd, (struct sockaddr*)& client_addr, &addr_size);
    if (connection_socket == -1) fatal_error("[CONNECTION ERROR]");
    
    return connection_socket;
}
