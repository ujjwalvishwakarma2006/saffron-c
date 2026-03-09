#include "common.h"


// ----------------------------- Application State -----------------------------
AppMode app_mode = NONE;
sem_t printing;
char* server_ip = "127.0.0.1";      
char* display_name = "Server";    
int msg_port = 8080;
int file_port = 8081;
int msg_socket, file_socket;


// -------------------------------- I/O Buffers --------------------------------
char msg_buf_in[SIZE];
char file_buf_in[SIZE];
char buf_out[SIZE];


// ------------------------------- Key File Paths -------------------------------
char* root_ca_cert_path = "root_ca_certificate.pem";
char* server_cert_path = "server_certificate.pem";
char* priv_key_path = "privkey.pem";
char* sym_key_path = "aeskey.bin";
char* sym_key_enc_path = "aeskey_enc.bin";


// ---------------------------- Temporary File Paths ----------------------------
char* msg_out_path = ".msg_out_unencrypted.txt";
char* msg_out_enc_path = ".msg_out_encrypted.bin";
char* msg_in_enc_path = ".msg_inc_encrypted.bin";
char* msg_in_path = ".msg_inc_unencrypted.txt";
char* file_out_enc_path = ".file_out_encrypted.bin";
char* file_in_enc_path = ".file_inc_encrypted.bin";


// ------------------------------- ncurses Windows -------------------------------
WINDOW *log_win = NULL, *input_win = NULL;


// ----------------------------------- Utility -----------------------------------
void fatal_error(char* error) {
    close(msg_socket);
    close(file_socket);
    printw("Some Error Occured. "
        "Press any key to see the error message on regular terminal.");
    getch();
    endwin();
    perror(error);
    exit(1);
}