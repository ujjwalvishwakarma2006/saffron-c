#ifndef MSG_RECV_H
#define MSG_RECV_H

/* Displays one received and already-decrypted message in the chat log. */
void display_received_message();

/* Thread loop for incoming messages:
 * receive -> verify -> decrypt -> display */
void* msg_recv_loop(void*);

#endif // !MSG_RECV_H