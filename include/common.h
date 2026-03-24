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

#define BUF_SIZE 1024                   /* Shared fixed-size transfer buffer */

typedef enum {
    SERVER,                             /* Application runs in server mode */
    CLIENT,                             /* Application runs in client mode */
    NONE
} AppMode;


// ----------------------------- Application State -----------------------------
extern AppMode app_mode;                /* Current process role */
extern sem_t printing;                  /* Synchronizes ncurses output across threads */
extern char* server_ip;                 /* Server bind/target IP */
extern char* peer_display_name;         /* Remote peer name shown in chat */
extern int msg_port, file_port;         /* Message and file channel ports */
extern int msg_socket, file_socket;     /* Connected sockets for both channels */


// -------------------------------- I/O Buffers --------------------------------
extern char message_input_buffer[BUF_SIZE]; /* Incoming message/file chunks */
extern char file_input_buffer[BUF_SIZE];    /* Incoming file chunks */
extern char output_buffer[BUF_SIZE];        /* Outgoing transfer chunks */


// ------------------------------- Permanent Paths -------------------------------
extern char* root_ca_cert_path;         /* Trusted root CA certificate */
extern char* server_cert_path;          /* Server certificate */
extern char* client_cert_path;          /* Client certificate */
extern char* server_secret_key_path;    /* Server RSA private key */
extern char* client_secret_key_path;    /* Client RSA private key */


// ------------------------------- Temporary Paths -------------------------------
extern char* dh_params_path;                /* Diffie-Hellman parameter file */
extern char* server_dh_public_key_path;     /* Server DH public key file */
extern char* server_dh_secret_key_path;     /* Server DH private key file */
extern char* client_dh_public_key_path;     /* Client DH public key file */
extern char* client_dh_secret_key_path;     /* Client DH private key file */
extern char* session_key_path;              /* Derived shared session key */


// ---------------------------- Temporary File Paths ----------------------------
// Intermediate files used in secure send/receive pipelines.
extern char* msg_out_path;              /* Outgoing message plaintext */
extern char* msg_out_enc_path;          /* Outgoing encrypted message */
extern char* msg_out_signed_path;       /* Outgoing encrypted + signed message */

extern char* msg_in_signed_path;        /* Incoming encrypted + signed message */
extern char* msg_in_enc_path;           /* Incoming encrypted message */
extern char* msg_in_path;               /* Incoming decrypted message */

extern char* file_out_path;             /* Outgoing file plaintext */
extern char* file_out_enc_path;         /* Outgoing encrypted file */
extern char* file_out_signed_path;      /* Outgoing encrypted + signed file */

extern char* file_in_signed_path;       /* Incoming encrypted + signed file */
extern char* file_in_enc_path;          /* Incoming encrypted file */
extern char* file_in_path;              /* Incoming decrypted file */


// ------------------------------- ncurses Windows -------------------------------
extern WINDOW *log_win, *input_win;     /* Chat log and input windows */


// ----------------------------------- Utility -----------------------------------
/* Gracefully shuts down ncurses, closes sockets, prints the error, and exits. */
void fatal_error(char* error);

#endif // !COMMON_H