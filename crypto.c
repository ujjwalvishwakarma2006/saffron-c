#include "common.h"
#include "crypto.h"

// Only AES-256-CBC Mode
void encrypt_file(char* file_name, char* enc_file_name, char* key_file) {
    int child_pid = fork(); 
    if (child_pid == 0) {

        char* args[] = {
            "openssl", 
            "enc", "-aes-256-cbc", "-salt", 
            "-in", file_name, 
            "-out", enc_file_name, "-pbkdf2", 
            "-kfile", key_file, 
            NULL
        };

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

        char* args[] = {
            "openssl",
            "enc", "-aes-256-cbc", "-d",
            "-in", enc_file_name,
            "-out", file_name, "-pbkdf2",
            "-kfile", key_file,
            NULL
        };

        execvp(args[0], args);
        
        // Exit from the child process. Mandetory. 
        exit(0);
    }
    
    // Wait for child process to finish generating key.
    waitpid(child_pid, NULL, 0);    
}

// Generate HMAC for the input_file and store in tag_file
void generate_hmac(char* tag_file, char* input_file) {
    int child_pid = fork();
    if (child_pid == 0) {
        char mac_opt_args[128];
        snprintf(mac_opt_args, sizeof(mac_opt_args), "key:%s", session_key);

        char* args[] = {
            "openssl",
            "dgst", "-sha256",
            "-mac", "hmac",
            "-macopt", mac_opt_args, 
            "-out", tag_file, 
            input_file,
            NULL
        };

        execvp("openssl", args);

        fatal_error("ERROR GENERATING DGST");
    }

    // Wait for the child process to finish, so that we do not file not exit error
    waitpid(child_pid, NULL, 0);
}