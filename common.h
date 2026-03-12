#ifndef COMMON_H
#define COMMON_H

// Standard library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#include <fcntl.h>

// POSIX / System headers
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

// TUI (ncurses)
#include <ncurses.h>

#define BUF_SIZE 1024                   /* Consistent buffer size */
#define KEY_SIZE 65                     /* Symmetric key size */
#define TAG_SIZE 128                    /* HMAC tag size */

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
extern char session_key[KEY_SIZE];      /* Buffer for session key */


// -------------------------------- Tag Buffers --------------------------------
extern char local_msg_tag[TAG_SIZE];    /* Buffer for locally generated message tag */
extern char recvd_msg_tag[TAG_SIZE];    /* Buffer for received message tag */
extern char local_file_tag[TAG_SIZE];   /* Buffer for locally generated file tag */
extern char recvd_file_tag[TAG_SIZE];   /* Buffer for received file tag */


// ------------------------------- Key File Paths -------------------------------
extern char* root_ca_cert_path;         /* Path to root CA's certificate */
extern char* server_cert_path;          /* Path to server's certificate */
extern char* priv_key_path;             /* Path to server's private key */
extern char* sym_key_path;              /* AES session key (plaintext) */
extern char* sym_key_enc_path;          /* AES session key (encrypted with server's public key) */


// ---------------------------- Temporary File Paths ----------------------------
// Intermediate files used during encryption/decryption within a session.
extern char* msg_out_path;              /* Outgoing message — plaintext */
extern char* msg_out_enc_path;          /* Outgoing message — encrypted */
extern char* msg_out_tag_path;          /* HMAC tag file of last sent message */

extern char* msg_in_enc_path;           /* Incoming message — encrypted */
extern char* msg_in_path;               /* Incoming message — decrypted */
extern char* msg_in_recvd_tag_path;     /* File of Received HMAC tag for last received msg */
extern char* msg_in_local_tag_path;     /* File of Generate HMAC tag for last received msg */

extern char* file_out_enc_path;         /* Outgoing file — encrypted */
extern char* file_out_tag_path;         /* HMAC tag file of last sent file */

extern char* file_in_enc_path;          /* Incoming file — encrypted */
extern char* file_in_recvd_tag_path;    /* File of Received HMAC tag for last received file */
extern char* file_in_local_tag_path;    /* File of Generate HMAC tag for last received file */


// ------------------------------- ncurses Windows -------------------------------
extern WINDOW *log_win, *input_win;     /* Log (chat history) and input (typing area) windows */


// ----------------------------------- Utility -----------------------------------
/* Gracefully shuts down ncurses, closes sockets, prints the error, and exits.
 * It is used instead of raw exit() to avoid leaving the terminal in a broken state. */
void fatal_error(char* error);

#endif // !COMMON_H