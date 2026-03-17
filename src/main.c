#include "common.h"
#include "args.h"
#include "tui.h"
#include "server.h"
#include "client.h"
#include "key_exchange.h"
#include "outgoing.h"
#include "recv_file.h"
#include "recv_msg.h"

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
        msg_socket = server_accept(msg_socket_listen);
        file_socket = server_accept(file_socket_listen);
        
        // Server runs server-side steps of key exchange protocol 
        if (!server_send_certificate()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!server_recv_certificate()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!server_verify_certificate()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!server_generate_dh_params()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!server_generate_dh_pkey()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!server_sign_dh_packet()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!server_send_signed_dh()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!server_recv_signed_dh_pkey()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!server_extract_dh_pkey()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!server_derive_secret_key()) fatal_error("[KEY EXCHANGE FAILURE]");
    } else if (app_mode == CLIENT) {
        // Initialize two separate connections with the server
        msg_socket = client_connect(server_ip, msg_port, "Message Transfer");
        file_socket = client_connect(server_ip, file_port, "File Transfer");

        // Client runs client-side steps of key exchange protocol 
        if (!client_recv_certificate()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!client_verify_certificate()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!client_send_certificate()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!client_recv_signed_dh()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!client_extract_dh_packet()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!client_generate_dh_pkey()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!client_sign_dh_pkey()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!client_send_signed_dh_pkey()) fatal_error("[KEY EXCHANGE FAILURE]");
        if (!client_derive_secret_key()) fatal_error("[KEY EXCHANGE FAILURE]");
    } else {
        fatal_error("[ENVIRONMENT INIT FAILED]");
    }

    // Threads handle incoming Messages/Files
    pthread_create(&msg_recv_thread, NULL, msg_recv_loop, NULL);
    pthread_create(&file_recv_thread, NULL, file_recv_loop, NULL);

    // Original Process handles outgoing transfers
    handle_outgoing(msg_socket, file_socket);

    // End ncurse session with all its windows and subwindows
    endwin();
    return 0;
}