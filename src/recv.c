#include "common.h"
#include "recv.h"
#include "file_utils.h"

/* Receives one length-prefixed payload and writes it to filename. */
void recv_file_content(int connection_socket, char* filename, char* buffer) {
    int bytes_received;
    uint32_t content_bytes;
    uint32_t received_bytes;
    uint32_t bytes_to_receive;
    FILE* file_pointer;

    memset(buffer, 0, BUF_SIZE);

    file_pointer = open_file(filename, "wb");
    if (file_pointer == NULL) return;

    bytes_received = recv(connection_socket, &content_bytes, sizeof(content_bytes), MSG_WAITALL);
    if (bytes_received <= 0) fatal_error("[ERROR CONTENT LENGTH RECV]");

    received_bytes = 0;
    while (received_bytes < content_bytes) {
        /* Each recv call reads only what remains, capped at BUF_SIZE. */
        bytes_to_receive = (content_bytes - received_bytes > BUF_SIZE)
            ? BUF_SIZE
            : (content_bytes - received_bytes);

        bytes_received = recv(connection_socket, buffer, bytes_to_receive, MSG_WAITALL);
        if (bytes_received <= 0) {
            printf("[WARNING] Connection closed while receiving file\n");
            break;
        }

        fwrite(buffer, bytes_received, 1, file_pointer);
        received_bytes += bytes_received;
    }

    fclose(file_pointer);
}