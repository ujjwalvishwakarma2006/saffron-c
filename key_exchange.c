#include "common.h"
#include "send.h"
#include "recv.h"
#include "key_exchange.h"

/* Step 1a: Server sends its certificate to the client */
void server_send_certificate() {
    send_file_content(file_socket, server_cert_path, buf_out);
    sem_wait(&printing);
    wprintw(log_win, "Certificate Sent\n");
    sem_post(&printing);
    wrefresh(log_win);
}

/* Step 1b: Client receives the certificate */
void client_recv_certificate() {
    recv_file_content(file_socket, server_cert_path, file_buf_in);
    sem_wait(&printing);
    wprintw(log_win, "Server Certificate Received\n");
    sem_post(&printing);
    wrefresh(log_win);
}

/* Step 2: Client verifies the certificate via root CA's certificate */
void client_verify_certificate() {
    int child_pid = fork();
    if (child_pid == 0) {
        // Open /dev/null for writing
        int dev_null = open("/dev/null", O_WRONLY);
        if (dev_null == -1) {
            perror("Failed to open /dev/null");
            exit(1);
        }

        // Redirect stdout (1) and stderr (2) to /dev/null
        dup2(dev_null, STDOUT_FILENO);
        dup2(dev_null, STDERR_FILENO);
        close(dev_null); // Don't need the extra descriptor anymore
        
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
    wrefresh(log_win);
}

/* Step 3: Client generates a session key for further communication */
void client_generate_session_key() {
    int child_pid = fork(); 
    if (child_pid == 0) {
        char* args[] = {"openssl", "rand", "-hex", "-out", sym_key_path, "32", NULL};
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

    // Read session key into the session_key buffer
    FILE* fp = fopen(sym_key_path, "r");
    fread(session_key, 1, 65, fp);
    fclose(fp);
}

/* Step 4: Client encrypts the session key using server's certificate */
void client_encrypt_session_key() {
    int child_pid = fork(); 
    if (child_pid == 0) {
        char* args[] = {"openssl", "pkeyutl", "-encrypt", "-pubin", "-inkey", server_cert_path, "-certin", "-in", sym_key_path, "-out", sym_key_enc_path, NULL};
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

/* Step 5a: Client sends the encrypted session key to the server */
void client_send_session_key() {
    send_file_content(file_socket, sym_key_enc_path, buf_out);
    sem_wait(&printing);
    wprintw(log_win, "Session Key sent\n");
    sem_post(&printing);
    wrefresh(log_win);
}

/* Step 5b: Server receives the encrypted session key */
void server_recv_session_key() {
    recv_file_content(file_socket, sym_key_enc_path, file_buf_in);
    sem_wait(&printing);
    wprintw(log_win, "Session Key Received\n");
    sem_post(&printing);
    wrefresh(log_win);
}

/* Step 6: Server decrypts the encrypted session key using its private key */
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

    // Read session key into the session_key buffer
    FILE* fp = fopen(sym_key_path, "r");
    fread(session_key, 1, 65, fp);
    fclose(fp);
}