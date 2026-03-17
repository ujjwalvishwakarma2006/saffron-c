#ifndef MSG_RECV_H
#define MSG_RECV_H

/* Receive (encrypted + signed) message */
void recv_signed_enc_msg();

/* Extract the encrypted message if it's signature is verified */
void extract_enc_msg();

/* Decrypt the encrypted message */
void msg_decrypt();

/* Display the decrypted message */
void msg_display();

/* Thread function: receive, verify, decrypt and display messages in a loop */
void* msg_recv_loop(void*);

#endif // !MSG_RECV_H