#ifndef COMMON_H
#define COMMON_H

// Standard library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#include <fcntl.h>
#include <errno.h>

// POSIX / System headers
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

// TUI (ncurses)
#include <ncurses.h>

#define BUF_SIZE 1024                   /* Consistent buffer size */

typedef enum {
    SERVER,                             /* Applications runs in SERVER mode */
    CLIENT,                             /* Applications runs in CLIENT mode */
    NONE
} AppMode;


// ----------------------------- Application State -----------------------------
extern AppMode app_mode;                /* Whether the app is running as SERVER or CLIENT */
extern sem_t printing;                  /* Semaphore to synchronize terminal output across threads */
extern char* server_ip;                 /* IP address of the server to connect to or bind on */
extern char* display_name;              /* Display name of the remote peer shown in chat */
extern int msg_port, file_port;         /* Ports for message and file transfer channels */
extern int msg_socket, file_socket;     /* Connected sockets for message and file transfer */


// -------------------------------- I/O Buffers --------------------------------
extern char msg_buf_in[BUF_SIZE];       /* Buffer for incoming message data */
extern char file_buf_in[BUF_SIZE];      /* Buffer for incoming file data */
extern char buf_out[BUF_SIZE];          /* Buffer for outgoing message/file data */


// ------------------------------- Permanent Paths -------------------------------
extern char* root_ca_cert_path;         /* Path to root CA's certificate */
extern char* server_cert_path;          /* Path to server's certificate */
extern char* client_cert_path;          /* Path to client's certificate */
extern char* server_skey_path;          /* Path to server's RSA secret key */
extern char* client_skey_path;          /* Path to client's RSA secret key */


// ------------------------------- Temporary Paths -------------------------------
extern char* dh_param_path;             /* Path to DH-parameters file */
extern char* server_dh_pkey_path;       /* Server's DH public key path */
extern char* server_dh_skey_path;       /* Server's DH secret/private key path */
extern char* client_dh_pkey_path;       /* Client's DH public key path */
extern char* client_dh_skey_path;       /* Client's DH secret/private key path */
extern char* session_key_path;          /* AES session key (plaintext) */


// ---------------------------- Temporary File Paths ----------------------------
// Intermediate files used during encryption/decryption within a session.
extern char* msg_out_path;              /* Outgoing message — plaintext */
extern char* msg_out_enc_path;          /* Outgoing message — encrypted */
extern char* msg_out_signed_path;       /* Outgoing message — (encrypted + signed) */

extern char* msg_in_signed_path;        /* Incoming message — (encrypted + signed) */
extern char* msg_in_enc_path;           /* Incoming message — encrypted */
extern char* msg_in_path;               /* Incoming message — decrypted */

extern char* file_out_path;             /* Outgoing file — plaintext */
extern char* file_out_enc_path;         /* Outgoing file — encrypted */
extern char* file_out_signed_path;      /* Outgoing file — (encrypted + signed) */

extern char* file_in_signed_path;       /* Incoming file — (encrypted + signed) */
extern char* file_in_enc_path;          /* Incoming file — encrypted */
extern char* file_in_path;              /* Incoming file — unencrypted */


// ------------------------------- ncurses Windows -------------------------------
extern WINDOW *log_win, *input_win;     /* Log (chat history) and input (typing area) windows */


// ----------------------------------- Utility -----------------------------------
/* Gracefully shuts down ncurses, closes sockets, prints the error, and exits.
 * It is used instead of raw exit() to avoid leaving the terminal in a broken state. */
void fatal_error(char* error);

#endif // !COMMON_H