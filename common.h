#ifndef COMMON_H
#define COMMON_H

// Standard library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>

// POSIX / System headers
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

// TUI (ncurses)
#include <ncurses.h>

#define SIZE 1024                       /* Consistent buffer size */

typedef enum {
    SERVER,
    CLIENT,
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
extern char msg_buf_in[SIZE];           /* Buffer for incoming message data */
extern char file_buf_in[SIZE];          /* Buffer for incoming file data */
extern char buf_out[SIZE];              /* Buffer for outgoing message/file data */


// ------------------------------- Key File Paths -------------------------------
extern char* pub_key_path;              /* Server's RSA public key  (PEM) */
extern char* priv_key_path;             /* Server's RSA private key (PEM) */
extern char* sym_key_path;              /* AES session key (plaintext) */
extern char* sym_key_enc_path;          /* AES session key (encrypted with server's public key) */


// ---------------------------- Temporary File Paths ----------------------------
// Intermediate files used during encryption/decryption within a session.
extern char* msg_out_path;              /* Outgoing message — plaintext */
extern char* msg_out_enc_path;          /* Outgoing message — encrypted */
extern char* msg_in_enc_path;           /* Incoming message — encrypted */
extern char* msg_in_path;               /* Incoming message — decrypted */
extern char* file_out_enc_path;         /* Outgoing file — encrypted */
extern char* file_in_enc_path;          /* Incoming file — encrypted */


// ------------------------------- ncurses Windows -------------------------------
extern WINDOW *log_win, *input_win;     /* Log (chat history) and input (typing area) windows */


// ----------------------------------- Utility -----------------------------------
/* Gracefully shuts down ncurses, closes sockets, prints the error, and exits.
 * It is used instead of raw exit() to avoid leaving the terminal in a broken state. */
void fatal_error(char* error);

#endif // !COMMON_H