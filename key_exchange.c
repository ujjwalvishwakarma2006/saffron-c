#include "common.h"
#include "key_exchange.h"

// ===============================================================================================
//                                     KEY EXCHANGE FUNCTIONS 
// ===============================================================================================

// Server calls the following function to share his public key
void send_server_public_key() {
    // Follow similar step to file_send() to send the public key file to the client
    int n;
    FILE* fp;
    uint32_t file_size;
    int connection_socket = file_socket;

    fp = fopen(pub_key_path, "rb");
    if (fp == NULL) fatal_error("[FILE OPENING ERROR - KEY EXCHANGE]");

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    n = send(connection_socket, &file_size, sizeof(file_size), 0);
    if (n == -1) fatal_error("[FILE LENGTH SEND ERROR - KEY EXCHANGE]");

    while ((n = fread(buf_out, 1, SIZE, fp)) > 0) {
        send(connection_socket, buf_out, n, 0);
    }

    fclose(fp);
    sem_wait(&printing);
    wprintw(log_win, "Public Key sent\n");
    sem_post(&printing);
    wrefresh(log_win);
}

// Client calls this function to receive server's public key
void recv_server_public_key() {
    int n;
    uint32_t file_size;
    uint32_t bytes_received, to_receive;
    FILE* fp;
    int connection_socket = file_socket;

    fp = fopen(pub_key_path, "w");
    if (fp == NULL) fatal_error("[FILE OPENING ERROR - KEY EXCHANGE]");

    n = recv(connection_socket, &file_size, sizeof(file_size), MSG_WAITALL);
    if (n <= 0) fatal_error("[FILESIZE RECV ERROR]");

    bytes_received = 0;
    while (bytes_received < file_size) {
        to_receive = (file_size-bytes_received > SIZE) ? 
                    SIZE : (file_size-bytes_received);
                    
        n = recv(connection_socket, file_buf_in, to_receive, 0);
        if (n <= 0) {
            printf("[WARNING] Connection closed while receiving file\n");
            break;
        }
        
        // Write to file
        fwrite(file_buf_in, 1, n, fp);
        bytes_received += n;
    }

    fclose(fp);
    sem_wait(&printing);
    wprintw(log_win, "Server Public Key Received\n");
    sem_post(&printing);
    wrefresh(log_win);
}

// Client calls this function to generate a session for every new session
void generate_session_key() {
    int child_pid = fork(); 
    if (child_pid == 0) {
        // openssl rand -out sym_key_path 32
        char* args[] = {"openssl", "rand", "-out", sym_key_path, "32", NULL};
        execvp(args[0], args);

        // Exit from the child process. Mandetory. 
        exit(0);
    }

    // Wait for child process to finish generating key.
    waitpid(child_pid, NULL, 0);
}

// Client calls this function to encrypt the session key using server's public key
void encrypt_session_key() {
    int child_pid = fork(); 
    if (child_pid == 0) {
        // openssl rand -out sym_key_path 32
        char* args[] = {"openssl", "pkeyutl", "-encrypt", "-pubin", "-inkey", pub_key_path, "-in", sym_key_path, "-out", sym_key_enc_path, NULL};
        execvp(args[0], args);

        // Exit from the child process. Mandetory. 
        exit(0);
    }

    // Wait for child process to finish generating key.
    waitpid(child_pid, NULL, 0);
}

// Client calls this function to generate a session key and share with the server
void send_session_key() {
    // Follow similar step to file_send() to send the public key file to the client
    int n;
    FILE* fp;
    uint32_t file_size;
    int connection_socket = file_socket;

    fp = fopen(sym_key_enc_path, "rb");
    if (fp == NULL) fatal_error("[FILE OPENING ERROR - KEY EXCHANGE]");

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    n = send(connection_socket, &file_size, sizeof(file_size), 0);
    if (n == -1) fatal_error("[FILE LENGTH SEND ERROR - KEY EXCHANGE]");

    while ((n = fread(buf_out, 1, SIZE, fp)) > 0) {
        send(connection_socket, buf_out, n, 0);
    }

    fclose(fp);
    sem_wait(&printing);
    wprintw(log_win, "Session Key sent\n");
    sem_post(&printing);
    wrefresh(log_win);
}

// Server calls this function to receive the session key from client
void recv_session_key() {
    int n;
    uint32_t file_size;
    uint32_t bytes_received, to_receive;
    FILE* fp;
    int connection_socket = file_socket;

    fp = fopen(sym_key_enc_path, "w");
    if (fp == NULL) fatal_error("[FILE OPENING ERROR - KEY EXCHANGE]");

    n = recv(connection_socket, &file_size, sizeof(file_size), MSG_WAITALL);
    if (n <= 0) fatal_error("[FILESIZE RECV ERROR]");

    bytes_received = 0;
    while (bytes_received < file_size) {
        to_receive = (file_size-bytes_received > SIZE) ? 
                    SIZE : (file_size-bytes_received);
                    
        n = recv(connection_socket, file_buf_in, to_receive, 0);
        if (n <= 0) {
            printf("[WARNING] Connection closed while receiving file\n");
            break;
        }
        
        // Write to file
        fwrite(file_buf_in, 1, n, fp);
        bytes_received += n;
    }

    fclose(fp);
    sem_wait(&printing);
    wprintw(log_win, "Session Key Received\n");
    sem_post(&printing);
    wrefresh(log_win);
}

// Server calls this function to decrypt the encrypted session key
void decrypt_session_key() {
    int child_pid = fork(); 
    if (child_pid == 0) {
        // openssl rand -out sym_key_path 32
        char* args[] = {"openssl", "pkeyutl", "-decrypt", "-inkey", priv_key_path, "-in", sym_key_enc_path, "-out", sym_key_path, NULL};
        execvp(args[0], args);

        // Exit from the child process. Mandetory. 
        exit(0);
    }

    // Wait for child process to finish generating key.
    waitpid(child_pid, NULL, 0);
}