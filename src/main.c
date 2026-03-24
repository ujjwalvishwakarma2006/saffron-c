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
    pthread_t message_receiver_thread;
    pthread_t file_receiver_thread;

    tui_init();

    if (app_mode == SERVER) {
        int message_listen_socket = server_start(server_ip, msg_port, "Message Transfer");
        int file_listen_socket = server_start(server_ip, file_port, "File Transfer");

        msg_socket = server_accept(message_listen_socket);
        file_socket = server_accept(file_listen_socket);

        if (!run_server_key_exchange()) {
            fatal_error("[KEY EXCHANGE] Server handshake sequence failed");
        }
    } else if (app_mode == CLIENT) {
        msg_socket = client_connect(server_ip, msg_port, "Message Transfer");
        file_socket = client_connect(server_ip, file_port, "File Transfer");

        if (!run_client_key_exchange()) {
            fatal_error("[KEY EXCHANGE] Client handshake sequence failed");
        }
    } else {
        fatal_error("[ENVIRONMENT INIT FAILED]");
    }

    pthread_create(&message_receiver_thread, NULL, msg_recv_loop, NULL);
    pthread_create(&file_receiver_thread, NULL, file_recv_loop, NULL);

    handle_outgoing(msg_socket, file_socket);

    endwin();
    return 0;
}