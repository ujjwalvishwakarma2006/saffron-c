#include "common.h"
#include "crypto.h"

// Utility function to disable outputting on terminal
void disable_terminal_output() {
    // Open /dev/null to print openssl output 
    int dev_null = open("/dev/null", O_WRONLY);
    if (dev_null == -1) {
        perror("Failed to open /dev/null");
        exit(1);
    }

    // Redirect stdout (1) and stderr (2) to /dev/null
    dup2(dev_null, STDOUT_FILENO);
    dup2(dev_null, STDERR_FILENO);
    close(dev_null);
}

// Verify peer_cert against root_cert
void verify_certificate(char* root_cert, char* peer_cert) {
    int child_pid = fork();
    if (child_pid == 0) {
        disable_terminal_output();
        char* args[] = {"openssl", "verify", "-CAfile", root_cert, peer_cert, NULL};
        execvp(args[0], args);
        
        fatal_error("[`execvp` SYSCALL FAILURE]");
    }
    
    // Wait for child process to finish verifying the signature
    int status;
    waitpid(child_pid, &status, 0);

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code != 0) {
            fatal_error("[ERROR VERIFYING FILE SIGNATURE]");
        }
    } else {
        fatal_error("[CERTIFICATE VERIFICATION - CHILD PROCESS ABNORMAL EXIT]");
    }
}

// Generate Diffie-Hellman parameters in out_file
void generate_dh_params(char* dhp_file) {
    int child_pid = fork(); 
    if (child_pid == 0) {
        disable_terminal_output();
        char* args[] = {
            "openssl", "genpkey", "-genparam", 
            "-algorithm", "DH", 
            "-out", dhp_file, NULL
        };
        execvp(args[0], args);
        fatal_error("[`execvp` SYSCALL FAILURE]");
    }

    waitpid(child_pid, NULL, 0);
}

// Generate Diffie-Hellman public-private key-pair from DH parameters
void generate_dh_key_pair(char* dhp_file, char* skey_file, char* pkey_file) {
    int child_pid;
    
    // Step 1: First generate a secret(private) key
    child_pid = fork(); 
    if (child_pid == 0) {
        disable_terminal_output();
        char* args[] = {
            "openssl", "genpkey", 
            "-paramfile", dhp_file, 
            "-out", skey_file, NULL
        };
        execvp(args[0], args);
        fatal_error("[`execvp` SYSCALL FAILURE]");
    }
    waitpid(child_pid, NULL, 0);
    
    // Step 2: Extract pubkey from the secret key
    child_pid = fork(); 
    if (child_pid == 0) {
        disable_terminal_output();
        char* args[] = {
            "openssl", "pkey", 
            "-in", skey_file, 
            "-pubout", "-out", pkey_file, NULL
        };
        execvp(args[0], args);
        fatal_error("[`execvp` SYSCALL FAILURE]");
    }
    waitpid(child_pid, NULL, 0);
}

// Derive DH secret key, given both client and server public keys
void derive_dh_skey(char* host_dh_skey, char* peer_dh_pkey, char* dh_skey_file) {
    int child_pid = fork();
    if (child_pid == 0) {
        disable_terminal_output();
        char* args[] = {
            "openssl", "pkeyutl", "-derive",
            "-inkey", host_dh_skey,
            "-peerkey", peer_dh_pkey, 
            "-out", dh_skey_file, NULL
        };
        execvp(args[0], args);
        fatal_error("[`execvp` SYSCALL FAILURE]");
    }
    waitpid(child_pid, NULL, 0);
}

// Modern sign and concatenate the signature with the file into a single file 
void cms_sign_file(char* in_file, char* signer_cert, char* signer_skey, char* out_file) {
    int child_pid = fork(); 
    if (child_pid == 0) {
        disable_terminal_output();
        char* args[] = {
            "openssl", "cms", "-sign", "-binary", 
            "-in", in_file, 
            "-signer", signer_cert, 
            "-inkey", signer_skey, 
            "-out", out_file, NULL
        };
        execvp(args[0], args);
        fatal_error("[`execvp` SYSCALL FAILURE]");
    }
    waitpid(child_pid, NULL, 0);
}

// Verify the signature and extract the file after verification in one single step
void cms_extract_file(char* in_file, char* root_cert, char* out_file) {
    int child_pid = fork(); 
    if (child_pid == 0) {
        disable_terminal_output();
        char* args[] = {
            "openssl", "cms", "-verify", "-binary",
            "-in", in_file, 
            "-CAfile", root_cert, 
            "-out", out_file, NULL
        };
        execvp(args[0], args);
        fatal_error("[`execvp` SYSCALL FAILURE]");
    }
    waitpid(child_pid, NULL, 0);
}

// Only AES-256-CBC Mode
void encrypt_file(char* file_name, char* enc_file_name, char* key_file) {
    int child_pid = fork(); 
    if (child_pid == 0) {
        char* args[] = {
            "openssl", "enc", "-aes-256-cbc", "-salt", 
            "-in", file_name, 
            "-out", enc_file_name, "-pbkdf2", 
            "-kfile", key_file, NULL
        };
        execvp(args[0], args);
        fatal_error("[`execvp` SYSCALL FAILURE]");
    }
    waitpid(child_pid, NULL, 0);
}

// Only AES-256-CBC Mode
void decrypt_file(char* enc_file_name, char* file_name, char* key_file) {
    int child_pid = fork(); 
    if (child_pid == 0) {
        char* args[] = {
            "openssl", "enc", "-aes-256-cbc", "-d",
            "-in", enc_file_name,
            "-out", file_name, "-pbkdf2",
            "-kfile", key_file, NULL
        };
        execvp(args[0], args);
        fatal_error("[`execvp` SYSCALL FAILURE]");
    }
    waitpid(child_pid, NULL, 0);    
}