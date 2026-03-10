#ifndef MSG_RECV_H
#define MSG_RECV_H

void recv_msg_enc();    /* Receive encrypted message */
void recv_msg_tag();    /* Receive HMAC tag for the message */
void vrfy_msg_tag();    /* Verify the tag of the message */
void msg_decrypt();     /* Decrypt the encrypted message */
void msg_display();     /* Display the decrypted message */

void* msg_recv_loop();  /* Thread function: receive, verify, decrypt and display messages in a loop */

#endif // !MSG_RECV_H