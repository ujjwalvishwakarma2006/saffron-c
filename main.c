#include "common.h"
#include "args.h"
#include "tui.h"
#include "server.h"
#include "client.h"
#include "key_exchange.h"
#include "crypto.h"
#include "outgoing.h"
#include "msg_recv.h"
#include "file_recv.h"

int main(int argc, char* argv[]) {
    setup(argc, argv);

    sem_init(&printing, 0, 1);
    pthread_t msg_recv_thread, file_recv_thread;
    
    // Init ncurses windows
    tui_init();
    
    if (app_mode == SERVER) {
        // Initialize listening sockets 
        int msg_socket_listen, file_socket_listen;
        msg_socket_listen = server_start(server_ip, msg_port, "Message Transfer");
        file_socket_listen = server_start(server_ip, file_port, "File Transfer");
  
        // Accept first connections
        msg_socket = server_accept(msg_socket_listen, "Message Transfer");
        file_socket = server_accept(file_socket_listen, "File Transfer");

        send_server_public_key();
        recv_session_key();
        decrypt_session_key();
    } else if (app_mode == CLIENT) {
        // Initialize two separate connections with the server
        msg_socket = client_connect(server_ip, msg_port, "Message Transfer");
        file_socket = client_connect(server_ip, file_port, "File Transfer");

        recv_server_public_key();
        generate_session_key();
        encrypt_session_key();
        send_session_key();
    } else {
        fatal_error("[ENVIRONMENT INIT FAILED]");
    }

    // Threads handle incoming Messages/Files
    pthread_create(&msg_recv_thread, NULL, msg_recv, NULL);
    pthread_create(&file_recv_thread, NULL, file_recv, NULL);

    // Original Process handles outgoing transfers
    handle_outgoing(msg_socket, file_socket);

    // End ncurse session with all its windows and subwindows
    endwin();
    return 0;
}