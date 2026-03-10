#include "common.h"
#include "recv.h"

void recv_file_content(int connection_socket, char* filename, char* buffer) {
    int n;
    uint32_t content_bytes;     /* Size of the content to be received */
    uint32_t received_bytes;    /* Number of Received bytes */
    uint32_t to_receive;        /* Number of bytes to receive in recv() one call */
    FILE* fp;

    // Clear buffer 
    memset(buffer, 0, SIZE);

    // Open the file in write mode
    fp = fopen(filename, "w");
    if (fp == NULL) fatal_error("[ERROR OPENING FILE]");

    // Receive content_bytes
    n = recv(connection_socket, &content_bytes, sizeof(content_bytes), MSG_WAITALL);
    if (n <= 0) fatal_error("[ERROR CONTENT LENGTH RECV]");
    
    // Receive content_bytes number of bytes
    received_bytes = 0;

    while (received_bytes < content_bytes) {

        // Receive content in the buffer
        to_receive = (content_bytes-received_bytes > SIZE) ?
                    SIZE : (content_bytes-received_bytes);
                    
        n = recv(connection_socket, buffer, to_receive, MSG_WAITALL);
        if (n <= 0) {
            printf("[WARNING] Connection closed while receiving file\n");
            break;
        }

        // Write content from the buffer to the file
        fwrite(buffer, n, 1, fp);
        received_bytes += n;
    }

    // Close the file
    fclose(fp);
}