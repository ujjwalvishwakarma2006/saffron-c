#include "common.h"


// ----------------------------- Application State -----------------------------
AppMode app_mode = NONE;
sem_t printing;
char* server_ip = "127.0.0.1";
char* peer_display_name = "Server";
int msg_port = 8080;
int file_port = 8081;
int msg_socket, file_socket;


// -------------------------------- I/O Buffers --------------------------------
char message_input_buffer[BUF_SIZE];
char file_input_buffer[BUF_SIZE];
char output_buffer[BUF_SIZE];


// ------------------------------ Permanent Paths ------------------------------
char* root_ca_cert_path = ".saffron-certs/rootca_certificate.pem";
char* server_cert_path = ".saffron-certs/server_certificate.pem";
char* client_cert_path = ".saffron-certs/client_certificate.pem";
char* server_secret_key_path = ".saffron-certs/server_rsa_skey.pem";
char* client_secret_key_path = ".saffron-certs/client_rsa_skey.pem";


// ---------------------------- Temporary Key Paths ----------------------------
char* dh_params_path = ".saffron-temp/dhp.pem";
char* server_dh_public_key_path = ".saffron-temp/server_dh_pkey.pem";
char* client_dh_public_key_path = ".saffron-temp/client_dh_pkey.pem";
char* server_dh_secret_key_path = ".saffron-temp/server_dh_skey.pem";
char* client_dh_secret_key_path = ".saffron-temp/client_dh_skey.pem";
char* session_key_path = ".saffron-temp/session_key.pem";


// --------------------------- Temporary File Paths ----------------------------
char* msg_out_path = ".saffron-temp/msg.out";
char* msg_out_enc_path = ".saffron-temp/msg.out.enc";
char* msg_out_signed_path = ".saffron-temp/msg.out.enc.signed";

char* msg_in_signed_path = ".saffron-temp/msg.in.enc.signed";
char* msg_in_enc_path = ".saffron-temp/msg.in.enc";
char* msg_in_path = ".saffron-temp/msg.in";

char* file_out_path = ".saffron-temp/file.out";
char* file_out_enc_path = ".saffron-temp/file.out.enc";
char* file_out_signed_path = ".saffron-temp/file.out.enc.signed";

char* file_in_signed_path = ".saffron-temp/file.in.enc.signed";
char* file_in_enc_path = ".saffron-temp/file.in.enc";
char* file_in_path = ".saffron-temp/file.in";


// ------------------------------- ncurses Windows -------------------------------
WINDOW *log_win = NULL, *input_win = NULL;


// ----------------------------------- Utility -----------------------------------
void fatal_error(char* error) {
    close(msg_socket);
    close(file_socket);

    clear();
    refresh();
    printw("An error occurred. Press any key to print details in the regular terminal.");
    getch();

    endwin();
    perror(error);
    exit(1);
}