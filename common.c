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
char msg_buf_in[BUF_SIZE];
char file_buf_in[BUF_SIZE];
char buf_out[BUF_SIZE];
char session_key[KEY_SIZE];


// -------------------------------- Tag Buffers --------------------------------
char expected_msg_tag[TAG_SIZE];
char received_msg_tag[TAG_SIZE];
char expected_file_tag[TAG_SIZE];
char received_file_tag[TAG_SIZE];


// ------------------------------- Key File Paths -------------------------------
char* root_ca_cert_path         = ".saffron-certs/root_ca_certificate.pem";
char* server_cert_path          = ".saffron-certs/server_certificate.pem";
char* priv_key_path             = ".saffron-certs/privkey.pem";
char* session_key_path          = ".saffron-certs/aeskey.hex";
char* encrypted_session_key_path= ".saffron-certs/aeskey.enc.bin";


// ---------------------------- Temporary File Paths ----------------------------
char* msg_out_path              = ".saffron-temp/msg_out.txt";
char* msg_out_enc_path          = ".saffron-temp/msg_out.enc.bin";
char* msg_out_tag_path          = ".saffron-temp/msg_out.tag";

char* msg_in_enc_path           = ".saffron-temp/msg_in.enc.bin";
char* msg_in_path               = ".saffron-temp/msg_in.txt";
char* msg_in_received_tag_path  = ".saffron-temp/msg_in.tag";
char* msg_in_expected_tag_path  = ".saffron-temp/msg_in.tag.local";

char* file_out_enc_path         = ".saffron-temp/file_out.enc.bin";
char* file_out_tag_path         = ".saffron-temp/file_out.tag";

char* file_in_enc_path          = ".saffron-temp/file_in.enc.bin";
char* file_in_received_tag_path = ".saffron-temp/file_in.tag";
char* file_in_expected_tag_path = ".saffron-temp/file_in.tag.local";


// ------------------------------- ncurses Windows -------------------------------
WINDOW *log_win = NULL, *input_win = NULL;


// ----------------------------------- Utility -----------------------------------
void fatal_error(char* error) {
    close(msg_socket);
    close(file_socket);
    clear();
    refresh();
    printw("Some Error Occured. "
        "Press any key to see the error message on regular terminal.");
    getch();
    endwin();
    perror(error);
    exit(1);
}