#include "common.h"
#include "client.h"

// ===============================================================================================
//                                      CLIENT SPECIFC FUNCTIONS 
// ===============================================================================================

int client_connect(const char* server_ip, const int server_port, const char* label) {
    int connection_socket;
    int n;
    struct sockaddr_in server_addr;

    connection_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connection_socket == -1) fatal_error("[SOCKET INIT ERROR]");
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = server_port;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    
    n = connect(connection_socket, (struct sockaddr*)& server_addr, sizeof(server_addr));
    if (n == -1) fatal_error("[CONNECTION ERROR]");
    wprintw(log_win, "Connected to %s:%d for %s\n", server_ip, server_port, label);
    wrefresh(log_win);

    return connection_socket;
}