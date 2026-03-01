#include "common.h"
#include "crypto.h"

// Only AES-256-CBC Mode
void encrypt_file(char* file_name, char* enc_file_name, char* key_file) {
    int child_pid = fork(); 
    if (child_pid == 0) {
        // openssl rand -out sym_key_path 32
        char* args[] = {"openssl", "enc", "-aes-256-cbc", "-salt", "-in", file_name, "-out", enc_file_name, "-pbkdf2", "-kfile", key_file, NULL};
        execvp(args[0], args);
        
        // Exit from the child process. Mandetory. 
        exit(0);
    }
    
    // Wait for child process to finish generating key.
    waitpid(child_pid, NULL, 0);
}

// Only AES-256-CBC Mode
void decrypt_file(char* enc_file_name, char* file_name, char* key_file) {
    int child_pid = fork(); 
    if (child_pid == 0) {
        // openssl rand -out sym_key_path 32
        char* args[] = {"openssl", "enc", "-aes-256-cbc", "-d", "-in", enc_file_name, "-out", file_name, "-pbkdf2", "-kfile", key_file, NULL};
        execvp(args[0], args);
        
        // Exit from the child process. Mandetory. 
        exit(0);
    }
    
    // Wait for child process to finish generating key.
    waitpid(child_pid, NULL, 0);    
}
