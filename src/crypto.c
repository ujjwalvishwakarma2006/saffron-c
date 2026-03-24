#include "common.h"
#include "crypto.h"
#include "tui.h"

/* Redirect OpenSSL stdout/stderr so cryptographic child processes do not pollute TUI output. */
void disable_terminal_output() {
    int dev_null = open("/dev/null", O_WRONLY);
    if (dev_null == -1) {
        perror("Failed to open /dev/null");
        exit(1);
    }

    dup2(dev_null, STDOUT_FILENO);
    dup2(dev_null, STDERR_FILENO);
    close(dev_null);
}

/* Wait for child process completion and report OpenSSL failures in the log window. */
bool crypto_wait_and_check(int child_pid, const char* error_prefix) {
    int status;
    waitpid(child_pid, &status, 0);

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code == 0) return true;

        sem_wait(&printing);
        wattron(log_win, COLOR_PAIR(CP_RED));
        wprintw(log_win, "[%s]: Failed with OpenSSL exit code: %d\n", error_prefix, exit_code);
        wattroff(log_win, COLOR_PAIR(CP_RED));
        sem_post(&printing);
        wrefresh(log_win);
        return false;
    }

    sem_wait(&printing);
    wattron(log_win, COLOR_PAIR(CP_RED));
    wprintw(log_win, "[%s]: Process terminated abnormally\n", error_prefix);
    wattroff(log_win, COLOR_PAIR(CP_RED));
    sem_post(&printing);
    wrefresh(log_win);
    return false;
}

/* Runs one OpenSSL command in a child process and applies shared wait/error handling. */
bool execute_crypto_command(char* command_args[], const char* error_prefix, bool suppress_output) {
    int child_pid = fork();
    if (child_pid == 0) {
        if (suppress_output) disable_terminal_output();
        execvp(command_args[0], command_args);
        exit(1);
    }

    return crypto_wait_and_check(child_pid, error_prefix);
}

/* Verify peer certificate against the trusted root certificate. */
bool verify_certificate(char* root_cert, char* peer_cert) {
    char* command_args[] = {"openssl", "verify", "-CAfile", root_cert, peer_cert, NULL};
    return execute_crypto_command(command_args, "ERROR VERIFYING CERTIFICATE", true);
}

/* Generate Diffie-Hellman parameters that both peers use to create DH key pairs. */
bool generate_dh_params(char* dh_params_file) {
    char* command_args[] = {
        "openssl", "genpkey", "-genparam",
        "-algorithm", "DH",
        "-out", dh_params_file, NULL
    };

    return execute_crypto_command(command_args, "ERROR GENERATING DH PARAMETERS", true);
}

/* Generate DH private key first, then derive matching public key from that private key. */
bool generate_dh_key_pair(
    char* dh_params_file,
    char* secret_key_file,
    char* public_key_file
) {
    char* generate_secret_key_args[] = {
        "openssl", "genpkey",
        "-paramfile", dh_params_file,
        "-out", secret_key_file, NULL
    };

    if (!execute_crypto_command(generate_secret_key_args, "ERROR GENERATING DH KEY PAIR", true)) {
        return false;
    }

    char* extract_public_key_args[] = {
        "openssl", "pkey",
        "-in", secret_key_file,
        "-pubout", "-out", public_key_file, NULL
    };

    return execute_crypto_command(extract_public_key_args, "ERROR EXTRACTING DH PUBLIC KEY", true);
}

/* Derive the shared DH secret key from local private key and peer public key. */
bool derive_dh_skey(
    char* local_dh_secret_key_file,
    char* peer_dh_public_key_file,
    char* derived_secret_file
) {
    char* command_args[] = {
        "openssl", "pkeyutl", "-derive",
        "-inkey", local_dh_secret_key_file,
        "-peerkey", peer_dh_public_key_file,
        "-out", derived_secret_file, NULL
    };

    return execute_crypto_command(command_args, "ERROR DERIVING DH SECRET", true);
}

/* Sign a payload with CMS so receiver can verify integrity and signer identity. */
bool cms_sign_file(char* in_file, char* signer_cert, char* signer_skey, char* out_file) {
    char* command_args[] = {
        "openssl", "cms", "-sign", "-binary",
        "-in", in_file,
        "-signer", signer_cert,
        "-inkey", signer_skey,
        "-out", out_file, NULL
    };

    return execute_crypto_command(command_args, "ERROR SIGNING FILE", true);
}

/* Verify CMS signature and extract the original payload only if verification passes. */
bool cms_extract_file(char* in_file, char* root_cert, char* out_file) {
    char* command_args[] = {
        "openssl", "cms", "-verify", "-binary",
        "-in", in_file,
        "-CAfile", root_cert,
        "-out", out_file, NULL
    };

    return execute_crypto_command(command_args, "ERROR VERIFYING SIGNED FILE", true);
}

/* Encrypt a file with AES-256-CBC; PBKDF2 derives keying material from key_file content. */
bool encrypt_file(char* file_name, char* enc_file_name, char* key_file) {
    char* command_args[] = {
        "openssl", "enc", "-aes-256-cbc", "-salt",
        "-in", file_name,
        "-out", enc_file_name, "-pbkdf2",
        "-kfile", key_file, NULL
    };

    return execute_crypto_command(command_args, "ERROR ENCRYPTING FILE", false);
}

/* Decrypt a file with AES-256-CBC using the shared session key file. */
bool decrypt_file(char* enc_file_name, char* file_name, char* key_file) {
    char* command_args[] = {
        "openssl", "enc", "-aes-256-cbc", "-d",
        "-in", enc_file_name,
        "-out", file_name, "-pbkdf2",
        "-kfile", key_file, NULL
    };

    return execute_crypto_command(command_args, "ERROR DECRYPTING FILE", false);
}