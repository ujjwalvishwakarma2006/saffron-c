#include "common.h"
#include "key_exchange.h"

// ===============================================================================================
//                                     KEY EXCHANGE FUNCTIONS 
// ===============================================================================================

// Server calls the following function to share his public key
void server_send_certificate() {
    // Follow similar step to file_send() to send the public key file to the client
    int n;
    FILE* fp;
    uint32_t file_size;
    int connection_socket = file_socket;

    fp = fopen(server_cert_path, "rb");
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
void client_recv_certificate() {
    int n;
    uint32_t file_size;
    uint32_t bytes_received, to_receive;
    FILE* fp;
    int connection_socket = file_socket;

    fp = fopen(server_cert_path, "w");
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

// Client verifies the ceritificate sent by the server using Root CA's certificate
void client_verify_certificate() {
    int child_pid = fork(); 
    if (child_pid == 0) {
        // openssl rand -out sym_key_path 32
        char* args[] = {"openssl", "verify", "-CAfile", root_ca_cert_path, server_cert_path, NULL};
        execvp(args[0], args);
    
        // execvp only returns if it fails to execute
        perror("execvp failed");
        exit(1);
    }
    
    // Wait for child process to finish generating key.
    int status;
    waitpid(child_pid, &status, 0);

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code != 0) {
            fatal_error("[CERTIFICATE VERIFICATION FAILED]");
        }
    } else {
        fatal_error("[CERTIFICATE VERIFICATION - CHILD PROCESS ABNORMAL EXIT]");
    }

    sem_wait(&printing);
    wprintw(log_win, "Server certificate verified successfully\n");
    sem_post(&printing);
    wrefresh(log_win);
}

// Client calls this function to generate a session for every new session
void client_generate_session_key() {
    int child_pid = fork(); 
    if (child_pid == 0) {
        char* args[] = {"openssl", "rand", "-out", sym_key_path, "32", NULL};
        execvp(args[0], args);

        // execvp only returns if it fails to execute
        perror("execvp failed");
        exit(1);
    }

    int status;
    waitpid(child_pid, &status, 0);

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code != 0) {
            fatal_error("[SESSION KEY GENERATION FAILED]");
        }
    } else {
        fatal_error("[SESSION KEY GENERATION - CHILD PROCESS ABNORMAL EXIT]");
    }
}

// Client calls this function to encrypt the session key using server's public key
void client_encrypt_session_key() {
    int child_pid = fork(); 
    if (child_pid == 0) {
        char* args[] = {"openssl", "pkeyutl", "-encrypt", "-certin", "-inkey", server_cert_path, "-in", sym_key_path, "-out", sym_key_enc_path, NULL};
        execvp(args[0], args);

        // execvp only returns if it fails to execute
        perror("execvp failed");
        exit(1);
    }

    int status;
    waitpid(child_pid, &status, 0);

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code != 0) {
            fatal_error("[SESSION KEY ENCRYPTION FAILED]");
        }
    } else {
        fatal_error("[SESSION KEY ENCRYPTION - CHILD PROCESS ABNORMAL EXIT]");
    }
}

// Client calls this function to generate a session key and share with the server
void client_send_session_key() {
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
void server_recv_session_key() {
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
void server_decrypt_session_key() {
    int child_pid = fork(); 
    if (child_pid == 0) {
        char* args[] = {"openssl", "pkeyutl", "-decrypt", "-inkey", priv_key_path, "-in", sym_key_enc_path, "-out", sym_key_path, NULL};
        execvp(args[0], args);

        // execvp only returns if it fails to execute
        perror("execvp failed");
        exit(1);
    }

    int status;
    waitpid(child_pid, &status, 0);

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code != 0) {
            fatal_error("[SESSION KEY DECRYPTION FAILED]");
        }
    } else {
        fatal_error("[SESSION KEY DECRYPTION - CHILD PROCESS ABNORMAL EXIT]");
    }
}