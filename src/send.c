#include "common.h"
#include "send.h"
#include "file_utils.h"

/* Sends one file by first transmitting its byte size and then raw file bytes. */
void send_file_content(int connection_socket, char* filepath, char* buffer) {
    int bytes_sent;
    uint32_t content_bytes;
    FILE* file_pointer;

    file_pointer = open_file(filepath, "rb");
    if (file_pointer == NULL) return;

    fseek(file_pointer, 0, SEEK_END);
    content_bytes = ftell(file_pointer);
    fseek(file_pointer, 0, SEEK_SET);

    bytes_sent = send(connection_socket, &content_bytes, sizeof(content_bytes), 0);
    if (bytes_sent == -1) fatal_error("[ERROR SEND CONTENT LENGTH]");

    while ((bytes_sent = fread(buffer, 1, BUF_SIZE, file_pointer)) > 0) {
        send(connection_socket, buffer, bytes_sent, 0);
    }

    fclose(file_pointer);
}